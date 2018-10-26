#include "libavplay.h"

extern "C" {
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"

#include "libavutil/time.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libavutil/display.h"

#include "colorspace.h"
}

#include <thread>

#include <SDL2/SDL.h>

#include <assert.h>

static AVDictionary *format_opts = nullptr;
static AVDictionary *codec_opts = nullptr;
static int avplay_initialized = 0;

int check_stream_specifier(AVFormatContext *s, AVStream *st, const char *spec)
{
    if (*spec <= '9' && *spec >= '0') /* opt:index */
        return strtol(spec, nullptr, 0) == st->index;
    else if (*spec == 'v' || *spec == 'a' || *spec == 's' || *spec == 'd' ||
             *spec == 't') { /* opt:[vasdt] */
        enum AVMediaType type;

        switch (*spec++) {
        case 'v': type = AVMEDIA_TYPE_VIDEO;      break;
        case 'a': type = AVMEDIA_TYPE_AUDIO;      break;
        case 's': type = AVMEDIA_TYPE_SUBTITLE;   break;
        case 'd': type = AVMEDIA_TYPE_DATA;       break;
        case 't': type = AVMEDIA_TYPE_ATTACHMENT; break;
        default:  av_assert0(0);
        }
        if (type != st->codecpar->codec_type)
            return 0;
        if (*spec++ == ':') { /* possibly followed by :index */
            int i, index = strtol(spec, nullptr, 0);
            for (i = 0; i < (int)s->nb_streams; i++)
                if (s->streams[i]->codecpar->codec_type == type && index-- == 0)
                   return i == st->index;
            return 0;
        }
        return 1;
    } else if (*spec == 'p' && *(spec + 1) == ':') {
        int prog_id, i, j;
        char *endptr;
        spec += 2;
        prog_id = strtol(spec, &endptr, 0);
        for (i = 0; i < (int)s->nb_programs; i++) {
            if (s->programs[i]->id != prog_id)
                continue;

            if (*endptr++ == ':') {
                int stream_idx = strtol(endptr, nullptr, 0);
                return stream_idx >= 0 &&
                    stream_idx < (int)s->programs[i]->nb_stream_indexes &&
                    st->index == (int)s->programs[i]->stream_index[stream_idx];
            }

            for (j = 0; j < (int)s->programs[i]->nb_stream_indexes; j++)
                if (st->index == (int)s->programs[i]->stream_index[j])
                    return 1;
        }
        return 0;
    } else if (*spec == 'i' && *(spec + 1) == ':') {
        int stream_id;
        char *endptr;
        spec += 2;
        stream_id = strtol(spec, &endptr, 0);
        return stream_id == st->id;
    } else if (*spec == 'm' && *(spec + 1) == ':') {
        AVDictionaryEntry *tag;
        char *key, *val;
        int ret;

        spec += 2;
        val = (char*)strchr(spec, ':');

        key = val ? av_strndup(spec, val - spec) : av_strdup(spec);
        if (!key)
            return AVERROR(ENOMEM);

        tag = av_dict_get(st->metadata, key, nullptr, 0);
        if (tag) {
            if (!val || !strcmp(tag->value, val + 1))
                ret = 1;
            else
                ret = 0;
        } else
            ret = 0;

        av_freep(&key);
        return ret;
    } else if (*spec == 'u') {
        AVCodecParameters *par = st->codecpar;
        int val;
        switch (par->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            val = par->sample_rate && par->channels;
            if (par->format == AV_SAMPLE_FMT_NONE)
                return 0;
            break;
        case AVMEDIA_TYPE_VIDEO:
            val = par->width && par->height;
            if (par->format == AV_PIX_FMT_NONE)
                return 0;
            break;
        case AVMEDIA_TYPE_UNKNOWN:
            val = 0;
            break;
        default:
            val = 1;
            break;
        }
        return par->codec_id != AV_CODEC_ID_NONE && val != 0;
    } else if (!*spec) /* empty specifier, matches everything */
        return 1;

    av_log(s, AV_LOG_ERROR, "Invalid stream specifier: %s.\n", spec);
    return AVERROR(EINVAL);
}

AVDictionary *filter_codec_opts(AVDictionary *opts, enum AVCodecID codec_id,
                                AVFormatContext *s, AVStream *st, AVCodec *codec)
{
    AVDictionary    *ret = nullptr;
    AVDictionaryEntry *t = nullptr;
    int            flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM
                                      : AV_OPT_FLAG_DECODING_PARAM;
    char          prefix = 0;
    const AVClass    *cc = avcodec_get_class();

    if (!codec)
        codec            = s->oformat ? avcodec_find_encoder(codec_id)
                                      : avcodec_find_decoder(codec_id);

    switch (st->codecpar->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        prefix  = 'v';
        flags  |= AV_OPT_FLAG_VIDEO_PARAM;
        break;
    case AVMEDIA_TYPE_AUDIO:
        prefix  = 'a';
        flags  |= AV_OPT_FLAG_AUDIO_PARAM;
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        prefix  = 's';
        flags  |= AV_OPT_FLAG_SUBTITLE_PARAM;
        break;
    default:
        break;
    }

    while ((t = av_dict_get(opts, "", t, AV_DICT_IGNORE_SUFFIX))) {
        char *p = strchr(t->key, ':');

        /* check stream specification in opt name */
        if (p)
            switch (check_stream_specifier(s, st, p + 1)) {
            case  1: *p = 0; break;
            case  0:         continue;
            default:         return nullptr;
            }

        if (av_opt_find(&cc, t->key, nullptr, flags, AV_OPT_SEARCH_FAKE_OBJ) ||
            (codec && codec->priv_class &&
             av_opt_find(&codec->priv_class, t->key, nullptr, flags,
                         AV_OPT_SEARCH_FAKE_OBJ)))
            av_dict_set(&ret, t->key, t->value, 0);
        else if (t->key[0] == prefix &&
                 av_opt_find(&cc, t->key + 1, nullptr, flags,
                             AV_OPT_SEARCH_FAKE_OBJ))
            av_dict_set(&ret, t->key + 1, t->value, 0);

        if (p)
            *p = ':';
    }
    return ret;
}

AVDictionary **setup_find_stream_info_opts(AVFormatContext *s,
                                           AVDictionary *codec_opts)
{
    int i;
    AVDictionary **opts;

    if (!s->nb_streams)
        return nullptr;
    opts = (AVDictionary**)av_mallocz(s->nb_streams * sizeof(*opts));
    if (!opts) {
        av_log(nullptr, AV_LOG_ERROR,
               "Could not alloc memory for stream options.\n");
        return nullptr;
    }
    for (i = 0; i < (int)s->nb_streams; i++)
        opts[i] = filter_codec_opts(codec_opts, s->streams[i]->codecpar->codec_id,
                                    s, s->streams[i], nullptr);
    return opts;
}

void init_pts_correction(PtsCorrectionContext *ctx)
{
    ctx->num_faulty_pts = ctx->num_faulty_dts = 0;
    ctx->last_pts = ctx->last_dts = INT64_MIN;
}

int64_t guess_correct_pts(PtsCorrectionContext *ctx, int64_t reordered_pts,
                          int64_t dts)
{
    int64_t pts = AV_NOPTS_VALUE;

    if (dts != (int64_t)AV_NOPTS_VALUE) {
        ctx->num_faulty_dts += dts <= ctx->last_dts;
        ctx->last_dts = dts;
    }
    if (reordered_pts != (int64_t)AV_NOPTS_VALUE) {
        ctx->num_faulty_pts += reordered_pts <= ctx->last_pts;
        ctx->last_pts = reordered_pts;
    }
    if ((ctx->num_faulty_pts<=ctx->num_faulty_dts || dts == (int64_t)AV_NOPTS_VALUE)
        && reordered_pts != (int64_t)AV_NOPTS_VALUE)
        pts = reordered_pts;
    else
        pts = dts;

    return pts;
}

static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    AVPacketList *pkt1;

    pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
    if (!pkt1)
        return -1;
    pkt1->pkt = *pkt;
    pkt1->next = nullptr;

    pthread_mutex_lock(&q->mutex);

    if (!q->last_pkt)

        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;
    q->last_pkt = pkt1;
    q->nb_packets++;
    q->size += pkt1->pkt.size + sizeof(*pkt1);
    /* XXX: should duplicate packet data in DV case */
    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
    return 0;
}

static void packet_queue_init(PlayerState *is, PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
    pthread_mutex_init(&q->mutex, nullptr);
    pthread_cond_init(&q->cond, nullptr);
    packet_queue_put(q, &is->flush_pkt);
}

static void packet_queue_flush(PacketQueue *q)
{
    AVPacketList *pkt, *pkt1;

    pthread_mutex_lock(&q->mutex);
    for (pkt = q->first_pkt; pkt != nullptr; pkt = pkt1) {
        pkt1 = pkt->next;
        av_packet_unref(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = nullptr;
    q->first_pkt = nullptr;
    q->nb_packets = 0;
    q->size = 0;
    pthread_mutex_unlock(&q->mutex);
}

static void packet_queue_end(PacketQueue *q)
{
    packet_queue_flush(q);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

static void packet_queue_abort(PacketQueue *q)
{
    pthread_mutex_lock(&q->mutex);

    q->abort_request = 1;

    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
    AVPacketList *pkt1;
    int ret;

    pthread_mutex_lock(&q->mutex);

    for (;;) {
        if (q->abort_request) {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = nullptr;
            q->nb_packets--;
            q->size -= pkt1->pkt.size + sizeof(*pkt1);
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&q->cond, &q->mutex);
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return ret;
}

static void video_image_display(PlayerState *is)
{
    VideoPicture *vp;
    // float aspect_ratio;
    // int width, height;

    vp = &is->pictq[is->pictq_rindex];

    if (vp->bmp[0]) {
        is->width = vp->width;
        is->height = vp->height;
        is->no_background = 0;
    
				if (is->render_callback != nullptr) {
					is->render_callback(is->render_callback_data, vp->bmp[vp->bmp_index], vp->width, vp->height);
				}
    }
}

// get the current audio output buffer size, in samples. With SDL, we cannot have a precise information 
static int audio_write_get_buf_size(PlayerState *is)
{
    return is->audio_buf_size - is->audio_buf_index;
}

// display the current picture, if any 
static void video_display(PlayerState *is)
{
    if (is->video_st)
        video_image_display(is);
}

// called to display each frame 
static void video_refresh_timer(void *opaque)
{
    PlayerState *is = (PlayerState*)opaque;
    VideoPicture *vp;

    // SubPicture *sp, *sp2;

    if (is->video_st) {
retry:
        if (is->pictq_size == 0) {
            // nothing to do, no picture to display in the que
        } else {
            double time = av_gettime_relative() / 1000000.0;
            double next_target;
            // dequeue the picture 
            vp = &is->pictq[is->pictq_rindex];

            if (time < vp->target_clock)
                return;
            // update current video pts 
            is->video_current_pts = vp->pts;
            is->video_current_pts_drift = is->video_current_pts - time;
            is->video_current_pos = vp->pos;
            if (is->pictq_size > 1) {
                VideoPicture *nextvp = &is->pictq[(is->pictq_rindex + 1) % VIDEO_PICTURE_QUEUE_SIZE];
                assert(nextvp->target_clock >= vp->target_clock);
                next_target= nextvp->target_clock;
            } else {
                next_target = vp->target_clock + is->video_clock - vp->pts; // FIXME pass durations cleanly
            }
            if (is->framedrop && time > next_target) {
                is->skip_frames *= 1.0 + FRAME_SKIP_FACTOR;
                if (is->pictq_size > 1 || time > next_target + 0.5) {
                    // update queue size and signal for next picture 
                    if (++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE)
                        is->pictq_rindex = 0;

                    pthread_mutex_lock(&is->pictq_mutex);
                    is->pictq_size--;
                    pthread_cond_signal(&is->pictq_cond);
                    pthread_mutex_unlock(&is->pictq_mutex);
                    goto retry;
                }
            }

            video_display(is);

            // update queue size and signal for next picture 
            if (++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE)
                is->pictq_rindex = 0;

            pthread_mutex_lock(&is->pictq_mutex);
            is->pictq_size--;
            pthread_cond_signal(&is->pictq_cond);
            pthread_mutex_unlock(&is->pictq_mutex);
        }
    } else if (is->audio_st) {
      // if only audio stream, then display the audio bars (better than nothing, just to test the implementation

      video_display(is);
    }
}

static void * refresh_thread(void *opaque)
{
    PlayerState *is= (PlayerState*)opaque;
    while (!is->abort_request) {
			if (is->refresh == 0) {
				video_refresh_timer(is);

				is->refresh = 0;
			}
      // FIXME ideally we should wait the correct time but SDLs event passing is so slow it would be silly
      av_usleep(is->audio_st ? is->rdftspeed * 1000 : 5000); 
			// av_usleep(5000); 
    }
    return nullptr;
}

// get the current audio clock value 
static double get_audio_clock(PlayerState *is)
{
    double pts;
    int hw_buf_size, bytes_per_sec;
    pts = is->audio_clock;
    hw_buf_size = audio_write_get_buf_size(is);
    bytes_per_sec = 0;
    if (is->audio_st) {
        bytes_per_sec = is->sdl_sample_rate * is->sdl_channels *
                        av_get_bytes_per_sample(is->sdl_sample_fmt);
    }
    if (bytes_per_sec)
        pts -= (double)hw_buf_size / bytes_per_sec;
    return pts;
}

// get the current video clock value 
static double get_video_clock(PlayerState *is)
{
    if (is->paused) {
        return is->video_current_pts;
    } else {
        return is->video_current_pts_drift + av_gettime_relative() / 1000000.0;
    }
}

// get the current external clock value 
static double get_external_clock(PlayerState *is)
{
    int64_t ti;
    ti = av_gettime_relative();
    return is->external_clock + ((ti - is->external_clock_time) * 1e-6);
}

// get the current master clock value 
static double get_master_clock(PlayerState *is)
{
    double val;

    if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
        if (is->video_st)
            val = get_video_clock(is);
        else
            val = get_audio_clock(is);
    } else if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
        if (is->audio_st)
            val = get_audio_clock(is);
        else
            val = get_video_clock(is);
    } else {
        val = get_external_clock(is);
    }
    return val;
}

static void stream_seek(PlayerState *is, int64_t pos, int64_t rel, int seek_by_bytes)
{
    if (!is->seek_req) {
        is->seek_pos = pos;
        is->seek_rel = rel;
        is->seek_flags &= ~AVSEEK_FLAG_BYTE;
        if (seek_by_bytes)
            is->seek_flags |= AVSEEK_FLAG_BYTE;
        is->seek_req = 1;
    }
}

static void stream_pause(PlayerState *is)
{
    if (is->paused) {
        is->frame_timer += av_gettime_relative() / 1000000.0 + is->video_current_pts_drift - is->video_current_pts;
        if (is->read_pause_return != AVERROR(ENOSYS)) {
            is->video_current_pts = is->video_current_pts_drift + av_gettime_relative() / 1000000.0;
        }
        is->video_current_pts_drift = is->video_current_pts - av_gettime_relative() / 1000000.0;
    }
    is->paused = !is->paused;
}

static double compute_target_time(double frame_current_pts, PlayerState *is)
{
    double delay, sync_threshold, diff = 0;

    // compute nominal delay 
    delay = frame_current_pts - is->frame_last_pts;
    if (delay <= 0 || delay >= 10.0) {
        // if incorrect delay, use previous one 
        delay = is->frame_last_delay;
    } else {
        is->frame_last_delay = delay;
    }
    is->frame_last_pts = frame_current_pts;

    // update delay to follow master synchronisation source 
    if (((is->av_sync_type == AV_SYNC_AUDIO_MASTER && is->audio_st) ||
         is->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {
        // if video is slave, we try to correct big delays by duplicating or deleting a frame 
        diff = get_video_clock(is) - get_master_clock(is);

        // skip or repeat frame. We take into account the delay to compute the threshold. I still don't know if it is the best guess 
        sync_threshold = FFMAX(AV_SYNC_THRESHOLD, delay);
        if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
            if (diff <= -sync_threshold)
                delay = 0;
            else if (diff >= sync_threshold)
                delay = 2 * delay;
        }
    }
    is->frame_timer += delay;

    av_log(nullptr, AV_LOG_TRACE, "video: delay=%0.3f pts=%0.3f A-V=%f\n", delay, frame_current_pts, -diff);

    return is->frame_timer;
}

static void player_close(PlayerState *is)
{
    VideoPicture *vp;
    int i;
    // XXX: use a special url_shutdown call to abort parse cleanly
    is->abort_request = 1;
    pthread_join(is->parse_tid, nullptr);
    pthread_join(is->refresh_tid, nullptr);

    // free all pictures 
    for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++) {
        vp = &is->pictq[i];
        if (vp->bmp[0]) {
            delete [] vp->bmp[0];
            vp->bmp[0] = nullptr;
        }
        if (vp->bmp[1]) {
            delete [] vp->bmp[1];
            vp->bmp[1] = nullptr;
        }
    }
    pthread_mutex_destroy(&is->video_filter_mutex);
    pthread_mutex_destroy(&is->pictq_mutex);
    pthread_cond_destroy(&is->pictq_cond);
    pthread_mutex_destroy(&is->subpq_mutex);
    pthread_cond_destroy(&is->subpq_cond);
}

/* allocate a picture (needs to do that in main thread to avoid
   potential locking problems */
static bool alloc_picture(void *opaque)
{
    PlayerState *is = (PlayerState*)opaque;
    VideoPicture *vp;

    vp = &is->pictq[is->pictq_windex];

    if (vp->bmp[0]) {
        delete [] vp->bmp[0];
        vp->bmp[0] = nullptr;
    }

    if (vp->bmp[1]) {
        delete [] vp->bmp[1];
        vp->bmp[1] = nullptr;
    }

    vp->width   = is->out_video_filter->inputs[0]->w;
    vp->height  = is->out_video_filter->inputs[0]->h;
    vp->pix_fmt = AV_PIX_FMT_ARGB; // (AVPixelFormat)is->out_video_filter->inputs[0]->format;

		vp->bmp[0] = new uint8_t[4*vp->width*vp->height];
		vp->bmp[1] = new uint8_t[4*vp->width*vp->height];

    pthread_mutex_lock(&is->pictq_mutex);
    vp->allocated = 1;
    pthread_cond_signal(&is->pictq_cond);
    pthread_mutex_unlock(&is->pictq_mutex);

    return 0;
}

// The 'pts' parameter is the dts of the packet / pts of the frame and guessed if not known.
static int queue_picture(PlayerState *is, AVFrame *src_frame, double pts, int64_t pos)
{
    VideoPicture *vp;
    // AVPicture pict_src;

    // wait until we have space to put a new picture
    pthread_mutex_lock(&is->pictq_mutex);

    if (is->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE && !is->refresh)
        is->skip_frames = FFMAX(1.0 - FRAME_SKIP_FACTOR, is->skip_frames * (1.0 - FRAME_SKIP_FACTOR));

    while (is->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE &&
           !is->videoq.abort_request) {
        pthread_cond_wait(&is->pictq_cond, &is->pictq_mutex);
    }
    pthread_mutex_unlock(&is->pictq_mutex);

    if (is->videoq.abort_request)
        return -1;

    vp = &is->pictq[is->pictq_windex];

    vp->sar = src_frame->sample_aspect_ratio;

    /* alloc or resize hardware picture buffer */
    if (!vp->bmp[0] || vp->reallocate ||
        vp->width  != is->out_video_filter->inputs[0]->w ||
        vp->height != is->out_video_filter->inputs[0]->h) {

        vp->allocated  = 0;
        vp->reallocate = 0;

        if (is->out_video_filter) {
          // video_open(is); // Jeff:: the allocation must be done in the main thread to avoid locking problems
          alloc_picture(is);
        }

        if (is->videoq.abort_request)
            return -1;
    }

    /* if the frame is not skipped, then display it */
    if (vp->bmp[0]) {
        uint8_t *data[4];
        int linesize[4];

        // INFO:: buffer duplo para evitar que o libav sobrescreva o ponteiro enviado para o objeto cairo
        vp->bmp_index = (vp->bmp_index + 1) % 2;

        data[0] = vp->bmp[vp->bmp_index];
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;

        linesize[0] = vp->width*4;
        linesize[1] = 0;
        linesize[2] = 0;
        linesize[3] = 0;

        AVPixelFormat fmt = AV_PIX_FMT_RGB32;

        struct SwsContext *ctx = 
          sws_getContext(vp->width, vp->height, (AVPixelFormat)src_frame->format, vp->width, vp->height, fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);

        if (ctx != nullptr) {
          sws_scale(ctx, src_frame->data, src_frame->linesize, 0, vp->height, data, linesize);
        }

        // FIXME use direct rendering
        // av_image_copy(data, linesize, (const uint8_t **)src_frame->data, src_frame->linesize, vp->pix_fmt, vp->width, vp->height);

        vp->pts = pts;
        vp->pos = pos;

        /* now we can update the picture count */
        if (++is->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE)
            is->pictq_windex = 0;
        pthread_mutex_lock(&is->pictq_mutex);
        vp->target_clock = compute_target_time(vp->pts, is);

        is->pictq_size++;
        pthread_mutex_unlock(&is->pictq_mutex);
				
        // CHANGE:: callback the method flip()
    }
    return 0;
}

/* Compute the exact PTS for the picture if it is omitted in the stream.
 * The 'pts1' parameter is the dts of the packet / pts of the frame. */
static int output_picture2(PlayerState *is, AVFrame *src_frame, double pts1, int64_t pos)
{
    double frame_delay, pts;
    int ret;

    pts = pts1;

    if (pts != 0) {
        /* update video clock with pts, if present */
        is->video_clock = pts;
    } else {
        pts = is->video_clock;
    }
    /* update video clock for next frame */
    frame_delay = av_q2d(is->video_dec->time_base);
    /* For MPEG-2, the frame can be repeated, so we update the
       clock accordingly */
    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
    is->video_clock += frame_delay;

    ret = queue_picture(is, src_frame, pts, pos);
    av_frame_unref(src_frame);
    return ret;
}

static int get_video_frame(PlayerState *is, AVFrame *frame, int64_t *pts, AVPacket *pkt)
{
    int got_picture, i;

    if (packet_queue_get(&is->videoq, pkt, 1) < 0)
        return -1;

    if (pkt->data == is->flush_pkt.data) {
        avcodec_flush_buffers(is->video_dec);

        pthread_mutex_lock(&is->pictq_mutex);
        // Make sure there are no long delay timers (ideally we should just flush the que but thats harder)
        for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++) {
            is->pictq[i].target_clock= 0;
        }
        while (is->pictq_size && !is->videoq.abort_request) {
            pthread_cond_wait(&is->pictq_cond, &is->pictq_mutex);
        }
        is->video_current_pos = -1;
        pthread_mutex_unlock(&is->pictq_mutex);

        init_pts_correction(&is->pts_ctx);
        is->frame_last_pts = AV_NOPTS_VALUE;
        is->frame_last_delay = 0;
        is->frame_timer = (double)av_gettime_relative() / 1000000.0;
        is->skip_frames = 1;
        is->skip_frames_index = 0;
        return 0;
    }

    avcodec_decode_video2(is->video_dec, frame, &got_picture, pkt);

    if (got_picture) {
        if (is->decoder_reorder_pts == -1) {
            *pts = guess_correct_pts(&is->pts_ctx, frame->pts, frame->pkt_dts);
        } else if (is->decoder_reorder_pts) {
            *pts = frame->pts;
        } else {
            *pts = frame->pkt_dts;
        }

        if (*pts == (int64_t)AV_NOPTS_VALUE) {
            *pts = 0;
        }
        if (is->video_st->sample_aspect_ratio.num) {
            frame->sample_aspect_ratio = is->video_st->sample_aspect_ratio;
        }

        is->skip_frames_index += 1;
        if (is->skip_frames_index >= is->skip_frames) {
            is->skip_frames_index -= FFMAX(is->skip_frames, 1.0);
            return 1;
        }
        av_frame_unref(frame);
    }
    return 0;
}

static int configure_video_filters(AVFilterGraph *graph, PlayerState *is, const char *vfilters)
{
    char sws_flags_str[128];
    char buffersrc_args[256];
    int ret;
    AVFilterContext *filt_src = nullptr, *filt_out = nullptr, *last_filter;
    AVCodecContext *codec = is->video_dec;

    snprintf(sws_flags_str, sizeof(sws_flags_str), "flags=%d", SWS_BICUBIC);
    graph->scale_sws_opts = av_strdup(sws_flags_str);

    snprintf(buffersrc_args, sizeof(buffersrc_args), "%d:%d:%d:%d:%d:%d:%d",
             codec->width, codec->height, codec->pix_fmt,
             is->video_st->time_base.num, is->video_st->time_base.den,
             codec->sample_aspect_ratio.num, codec->sample_aspect_ratio.den);


    if ((ret = avfilter_graph_create_filter(&filt_src,
                                            avfilter_get_by_name("buffer"),
                                            "src", buffersrc_args, nullptr,
                                            graph)) < 0)
        return ret;
    if ((ret = avfilter_graph_create_filter(&filt_out,
                                            avfilter_get_by_name("buffersink"),
                                            "out", nullptr, nullptr, graph)) < 0)
        return ret;

    last_filter = filt_out;

// Note: this macro adds a filter before the lastly added filter, so the processing order of the filters is in reverse
#define INSERT_FILT(name, arg) do {                                          \
    AVFilterContext *filt_ctx;                                               \
                                                                             \
    ret = avfilter_graph_create_filter(&filt_ctx,                            \
                                       avfilter_get_by_name(name),           \
                                       "avplay_" name, arg, nullptr, graph);    \
    if (ret < 0)                                                             \
        return ret;                                                          \
                                                                             \
    ret = avfilter_link(filt_ctx, 0, last_filter, 0);                        \
    if (ret < 0)                                                             \
        return ret;                                                          \
                                                                             \
    last_filter = filt_ctx;                                                  \
} while (0)

    INSERT_FILT("format", "yuv420p");

    if (is->autorotate) {
        uint8_t* displaymatrix = av_stream_get_side_data(is->video_st,
                                                         AV_PKT_DATA_DISPLAYMATRIX, nullptr);
        if (displaymatrix) {
            double rot = av_display_rotation_get((int32_t*) displaymatrix);
            if (rot < -135 || rot > 135) {
                INSERT_FILT("vflip", nullptr);
                INSERT_FILT("hflip", nullptr);
            } else if (rot < -45) {
                INSERT_FILT("transpose", "dir=clock");
            } else if (rot > 45) {
                INSERT_FILT("transpose", "dir=cclock");
            }
        }
    }

    if (vfilters) {
        AVFilterInOut *outputs = avfilter_inout_alloc();
        AVFilterInOut *inputs  = avfilter_inout_alloc();

        outputs->name    = av_strdup("in");
        outputs->filter_ctx = filt_src;
        outputs->pad_idx = 0;
        outputs->next    = nullptr;

        inputs->name    = av_strdup("out");
        inputs->filter_ctx = last_filter;
        inputs->pad_idx = 0;
        inputs->next    = nullptr;

        if ((ret = avfilter_graph_parse(graph, vfilters, inputs, outputs, nullptr)) < 0)
            return ret;
    } else {
        if ((ret = avfilter_link(filt_src, 0, last_filter, 0)) < 0)
            return ret;
    }

    if ((ret = avfilter_graph_config(graph, nullptr)) < 0)
        return ret;

    is->in_video_filter  = filt_src;
    is->out_video_filter = filt_out;

    return ret;
}

static void * video_thread(void *arg)
{
    PlayerState *is = (PlayerState*)arg;
    AVPacket pkt = { 0 };
    AVFrame *frame = av_frame_alloc();
    int64_t pts_int;
    double pts;
    int ret;

    AVFilterGraph *graph = avfilter_graph_alloc();
    AVFilterContext *filt_out = nullptr, *filt_in = nullptr;
    int last_w = is->video_dec->width;
    int last_h = is->video_dec->height;
    if (!graph) {
        av_frame_free(&frame);
        return nullptr;
    }

    if ((ret = configure_video_filters(graph, is, is->vfilters)) < 0)
        goto the_end;
    filt_in  = is->in_video_filter;
    filt_out = is->out_video_filter;

    if (!frame) {
        avfilter_graph_free(&graph);
        return nullptr;
    }

    for (;;) {
        AVRational tb;
        while (is->paused && !is->videoq.abort_request)
            SDL_Delay(10);

        av_packet_unref(&pkt);

        ret = get_video_frame(is, frame, &pts_int, &pkt);
        if (ret < 0)
            goto the_end;

        if (!ret)
            continue;

        if (   last_w != is->video_dec->width
            || last_h != is->video_dec->height) {
            av_log(nullptr, AV_LOG_TRACE, "Changing size %dx%d -> %dx%d\n", last_w, last_h,
                    is->video_dec->width, is->video_dec->height);
            avfilter_graph_free(&graph);
            graph = avfilter_graph_alloc();
            if ((ret = configure_video_filters(graph, is, is->vfilters)) < 0)
                goto the_end;
            filt_in  = is->in_video_filter;
            filt_out = is->out_video_filter;
            last_w = is->video_dec->width;
            last_h = is->video_dec->height;
        }

        frame->pts = pts_int;
        ret = av_buffersrc_add_frame(filt_in, frame);
        if (ret < 0)
            goto the_end;

        while (ret >= 0) {
            ret = av_buffersink_get_frame(filt_out, frame);
            if (ret < 0) {
                ret = 0;
                break;
            }

            pts_int = frame->pts;
            tb      = filt_out->inputs[0]->time_base;
            if (av_cmp_q(tb, is->video_st->time_base)) {
                av_unused int64_t pts1 = pts_int;
                pts_int = av_rescale_q(pts_int, tb, is->video_st->time_base);
                av_log(nullptr, AV_LOG_TRACE, "video_thread(): "
                        "tb:%d/%d pts:%" PRId64" -> tb:%d/%d pts:%" PRId64"\n",
                        tb.num, tb.den, pts1,
                        is->video_st->time_base.num, is->video_st->time_base.den, pts_int);
            }
            pts = pts_int * av_q2d(is->video_st->time_base);
            ret = output_picture2(is, frame, pts, 0);
        }

        if (ret < 0)
            goto the_end;


        if (is && is->step)
                stream_pause(is);
    }
 the_end:
    pthread_mutex_lock(&is->video_filter_mutex);
    is->out_video_filter = nullptr;
    pthread_mutex_unlock(&is->video_filter_mutex);
    av_freep(&is->vfilters);
    avfilter_graph_free(&graph);
    av_packet_unref(&pkt);
    av_frame_free(&frame);
    return nullptr;
}

/* return the new audio buffer size (samples can be added or deleted
   to get better sync if video or external master clock) */
static int synchronize_audio(PlayerState *is, short *samples,
                             int samples_size1, double pts)
{
    int n, samples_size;
    double ref_clock;

    n = is->sdl_channels * av_get_bytes_per_sample(is->sdl_sample_fmt);
    samples_size = samples_size1;

    /* if not master, then we try to remove or add samples to correct the clock */
    if (((is->av_sync_type == AV_SYNC_VIDEO_MASTER && is->video_st) ||
         is->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {
        double diff, avg_diff;
        int wanted_size, min_size, max_size, nb_samples;

        ref_clock = get_master_clock(is);
        diff = get_audio_clock(is) - ref_clock;

        if (diff < AV_NOSYNC_THRESHOLD) {
            is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
            if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
                /* not enough measures to have a correct estimate */
                is->audio_diff_avg_count++;
            } else {
                /* estimate the A-V difference */
                avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);

                if (fabs(avg_diff) >= is->audio_diff_threshold) {
                    wanted_size = samples_size + ((int)(diff * is->sdl_sample_rate) * n);
                    nb_samples = samples_size / n;

                    min_size = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX)) / 100) * n;
                    max_size = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX)) / 100) * n;
                    if (wanted_size < min_size)
                        wanted_size = min_size;
                    else if (wanted_size > max_size)
                        wanted_size = max_size;

                    /* add or remove samples to correction the synchro */
                    if (wanted_size < samples_size) {
                        /* remove samples */
                        samples_size = wanted_size;
                    } else if (wanted_size > samples_size) {
                        uint8_t *samples_end, *q;
                        int nb;

                        /* add samples */
                        nb = (samples_size - wanted_size);
                        samples_end = (uint8_t *)samples + samples_size - n;
                        q = samples_end + n;
                        while (nb > 0) {
                            memcpy(q, samples_end, n);
                            q += n;
                            nb -= n;
                        }
                        samples_size = wanted_size;
                    }
                }
                av_log(nullptr, AV_LOG_TRACE, "diff=%f adiff=%f sample_diff=%d apts=%0.3f vpts=%0.3f %f\n",
                        diff, avg_diff, samples_size - samples_size1,
                        is->audio_clock, is->video_clock, is->audio_diff_threshold);
            }
        } else {
            /* too big difference : may be initial PTS errors, so
               reset A-V filter */
            is->audio_diff_avg_count = 0;
            is->audio_diff_cum       = 0;
        }
    }

    return samples_size;
}

/* decode one audio frame and returns its uncompressed size */
static int audio_decode_frame(PlayerState *is, double *pts_ptr)
{
    AVPacket *pkt_temp = &is->audio_pkt_temp;
    AVPacket *pkt = &is->audio_pkt;
    AVCodecContext *dec = is->audio_dec;
    int n, len1, data_size, got_frame;
    double pts;
    int new_packet = 0;
    int flush_complete = 0;

    for (;;) {
        /* NOTE: the audio packet can contain several frames */
        while (pkt_temp->size > 0 || (!pkt_temp->data && new_packet)) {
            int resample_changed, audio_resample;

            if (!is->frame) {
                if (!(is->frame = av_frame_alloc()))
                    return AVERROR(ENOMEM);
            }

            if (flush_complete)
                break;
            new_packet = 0;
            len1 = avcodec_decode_audio4(dec, is->frame, &got_frame, pkt_temp);
            if (len1 < 0) {
                /* if error, we skip the frame */
                pkt_temp->size = 0;
                break;
            }

            pkt_temp->data += len1;
            pkt_temp->size -= len1;

            if (!got_frame) {
                /* stop sending empty packets if the decoder is finished */
                if (!pkt_temp->data && (dec->codec->capabilities & AV_CODEC_CAP_DELAY))
                    flush_complete = 1;
                continue;
            }
            data_size = av_samples_get_buffer_size(nullptr, dec->channels,
                                                   is->frame->nb_samples,
                                                   (AVSampleFormat)is->frame->format, 1);

            audio_resample = is->frame->format         != is->sdl_sample_fmt     ||
                             is->frame->channel_layout != is->sdl_channel_layout ||
                             is->frame->sample_rate    != is->sdl_sample_rate;

            resample_changed = is->frame->format         != is->resample_sample_fmt     ||
                               is->frame->channel_layout != is->resample_channel_layout ||
                               is->frame->sample_rate    != is->resample_sample_rate;

            if ((!is->avr && audio_resample) || resample_changed) {
                int ret;
                if (is->avr)
                    avresample_close(is->avr);
                else if (audio_resample) {
                    is->avr = avresample_alloc_context();
                    if (!is->avr) {
                        fprintf(stderr, "error allocating AVAudioResampleContext\n");
                        break;
                    }
                }
                if (audio_resample) {
                    av_opt_set_int(is->avr, "in_channel_layout",  is->frame->channel_layout, 0);
                    av_opt_set_int(is->avr, "in_sample_fmt",      is->frame->format,         0);
                    av_opt_set_int(is->avr, "in_sample_rate",     is->frame->sample_rate,    0);
                    av_opt_set_int(is->avr, "out_channel_layout", is->sdl_channel_layout,    0);
                    av_opt_set_int(is->avr, "out_sample_fmt",     is->sdl_sample_fmt,        0);
                    av_opt_set_int(is->avr, "out_sample_rate",    is->sdl_sample_rate,       0);

                    if ((ret = avresample_open(is->avr)) < 0) {
                        fprintf(stderr, "error initializing libavresample\n");
                        break;
                    }
                }
                is->resample_sample_fmt     = (AVSampleFormat)is->frame->format;
                is->resample_channel_layout = is->frame->channel_layout;
                is->resample_sample_rate    = is->frame->sample_rate;
            }

            if (audio_resample) {
                void *tmp_out;
                int out_samples, out_size, out_linesize;
                int osize      = av_get_bytes_per_sample(is->sdl_sample_fmt);
                int nb_samples = is->frame->nb_samples;

                out_size = av_samples_get_buffer_size(&out_linesize,
                                                      is->sdl_channels,
                                                      nb_samples,
                                                      is->sdl_sample_fmt, 0);
                tmp_out = av_realloc(is->audio_buf1, out_size);
                if (!tmp_out)
                    return AVERROR(ENOMEM);
                is->audio_buf1 = (uint8_t*)tmp_out;

                out_samples = avresample_convert(is->avr,
                                                 &is->audio_buf1,
                                                 out_linesize, nb_samples,
                                                 is->frame->data,
                                                 is->frame->linesize[0],
                                                 is->frame->nb_samples);
                if (out_samples < 0) {
                    fprintf(stderr, "avresample_convert() failed\n");
                    break;
                }
                is->audio_buf = is->audio_buf1;
                data_size = out_samples * osize * is->sdl_channels;
            } else {
                is->audio_buf = is->frame->data[0];
            }

            /* if no pts, then compute it */
            pts = is->audio_clock;
            *pts_ptr = pts;
            n = is->sdl_channels * av_get_bytes_per_sample(is->sdl_sample_fmt);
            is->audio_clock += (double)data_size /
                (double)(n * is->sdl_sample_rate);
#ifdef DEBUG
            {
                static double last_clock;
                printf("audio: delay=%0.3f clock=%0.3f pts=%0.3f\n",
                       is->audio_clock - last_clock,
                       is->audio_clock, pts);
                last_clock = is->audio_clock;
            }
#endif
            return data_size;
        }

        /* free the current packet */
        if (pkt->data)
            av_packet_unref(pkt);
        memset(pkt_temp, 0, sizeof(*pkt_temp));

        if (is->paused || is->audioq.abort_request) {
            return -1;
        }

        /* read next packet */
        if ((new_packet = packet_queue_get(&is->audioq, pkt, 1)) < 0)
            return -1;

        if (pkt->data == is->flush_pkt.data) {
            avcodec_flush_buffers(dec);
            flush_complete = 0;
        }

        *pkt_temp = *pkt;

        /* if update the audio clock with the pts */
        if (pkt->pts != (int64_t)AV_NOPTS_VALUE) {
            is->audio_clock = av_q2d(is->audio_st->time_base)*pkt->pts;
        }
    }
}

/* prepare a new audio buffer */
static void sdl_audio_callback(void *opaque, Uint8 *stream, int len)
{
    PlayerState *is = (PlayerState*)opaque;
    int audio_size, len1;
    double pts;

    is->audio_callback_time = av_gettime_relative();

    while (len > 0) {
        if (is->audio_buf_index >= (int)is->audio_buf_size) {
           audio_size = audio_decode_frame(is, &pts);
           if (audio_size < 0) {
                /* if error, just output silence */
               is->audio_buf      = is->silence_buf;
               is->audio_buf_size = sizeof(is->silence_buf);
           } else {
               audio_size = synchronize_audio(is, (int16_t *)is->audio_buf, audio_size,
                                              pts);
               is->audio_buf_size = audio_size;
           }
           is->audio_buf_index = 0;
        }
        len1 = is->audio_buf_size - is->audio_buf_index;
        if (len1 > len)
            len1 = len;
        memcpy(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
        len -= len1;
        stream += len1;
        is->audio_buf_index += len1;
    }
}

static AVCodec *find_codec_or_die(const char *name, enum AVMediaType type)
{
    const AVCodecDescriptor *desc;
    AVCodec *codec = avcodec_find_decoder_by_name(name);

    if (!codec && (desc = avcodec_descriptor_get_by_name(name))) {
        codec = avcodec_find_decoder(desc->id);
        if (codec)
            av_log(nullptr, AV_LOG_VERBOSE, "Matched decoder '%s' for codec '%s'.\n",
                   codec->name, desc->name);
    }

    if (!codec) {
        av_log(nullptr, AV_LOG_FATAL, "Unknown decoder '%s'\n", name);

        return nullptr;
    }

    if (codec->type != type) {
        av_log(nullptr, AV_LOG_FATAL, "Invalid decoder type '%s'\n", name);

        return nullptr;
    }

    return codec;
}

static AVCodec *choose_decoder(PlayerState *is, AVFormatContext *ic, AVStream *st)
{
    char *codec_name = nullptr;
    int i, ret;

    for (i = 0; i < is->nb_codec_names; i++) {
        char *spec = is->codec_names[i].specifier;
        if ((ret = check_stream_specifier(ic, st, spec)) > 0)
            codec_name = (char*)is->codec_names[i].u.str;
        else if (ret < 0) {
          return nullptr;
        }
    }

    if (codec_name) {
        AVCodec *codec = find_codec_or_die(codec_name, st->codecpar->codec_type);
        st->codecpar->codec_id = codec->id;
        return codec;
    } else
        return avcodec_find_decoder(st->codecpar->codec_id);
}

/* open a given stream. Return 0 if OK */
static int stream_component_open(PlayerState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;
    AVCodec *codec;
    SDL_AudioSpec wanted_spec, spec;
    AVDictionary *opts;
    AVDictionaryEntry *t = nullptr;
    int ret = 0;

    if (stream_index < 0 || stream_index >= (int)ic->nb_streams)
        return -1;

    avctx = avcodec_alloc_context3(nullptr);
    if (!avctx)
        return AVERROR(ENOMEM);

    ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);
    if (ret < 0) {
        avcodec_free_context(&avctx);
        return ret;
    }

    opts = filter_codec_opts(codec_opts, avctx->codec_id, ic, ic->streams[stream_index], nullptr);

    codec = choose_decoder(is, ic, ic->streams[stream_index]);
    avctx->workaround_bugs   = is->workaround_bugs;
    avctx->idct_algo         = is->idct;
    avctx->skip_frame        = is->skip_frame;
    avctx->skip_idct         = is->skip_idct;
    avctx->skip_loop_filter  = is->skip_loop_filter;
    avctx->error_concealment = is->error_concealment;

    if (is->fast)
        avctx->flags2 |= AV_CODEC_FLAG2_FAST;

    if (!av_dict_get(opts, "threads", nullptr, 0))
        av_dict_set(&opts, "threads", "auto", 0);
    if (avctx->codec_type == AVMEDIA_TYPE_VIDEO)
        av_dict_set(&opts, "refcounted_frames", "1", 0);
    if (!codec ||
        (ret = avcodec_open2(avctx, codec, &opts)) < 0) {
        goto fail;
    }
    if ((t = av_dict_get(opts, "", nullptr, AV_DICT_IGNORE_SUFFIX))) {
        av_log(nullptr, AV_LOG_ERROR, "Option %s not found.\n", t->key);
        ret =  AVERROR_OPTION_NOT_FOUND;
        goto fail;
    }

		while ((t = av_dict_get(is->ic->metadata, "", t, AV_DICT_IGNORE_SUFFIX))) {
			if (strcasecmp(t->key, "album") == 0 ) {
				strncpy(is->album, t->value, 1024-1);
			} else if (strcasecmp(t->key, "artist") == 0 ) {
				strncpy(is->author, t->value, 1024-1);
			} else if (strcasecmp(t->key, "album_artist") == 0) {
				strncpy(is->author, t->value, 1024-1);
			} else if (strcasecmp(t->key, "composer") == 0) {
				strncpy(is->author, t->value, 1024-1);
			} else if (strcasecmp(t->key, "performer") == 0) {
				strncpy(is->author, t->value, 1024-1);
			} else if (strcasecmp(t->key, "title") == 0) {
				strncpy(is->title, t->value, 1024-1);
			} else if (strcasecmp(t->key, "genre") == 0) {
				strncpy(is->genre, t->value, 1024-1);
			} else if (strcasecmp(t->key, "comment") == 0) {
				strncpy(is->comments, t->value, 1024-1);
			} else if (strcasecmp(t->key, "date") == 0) {
				strncpy(is->date, t->value, 1024-1);
			} else if (strcasecmp(t->key, "creation_time") == 0) {
				strncpy(is->date, t->value, 1024-1);
			}
		}

    /* prepare audio output */
    if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        is->sdl_sample_rate = avctx->sample_rate;

        if (!avctx->channel_layout)
            avctx->channel_layout = av_get_default_channel_layout(avctx->channels);
        if (!avctx->channel_layout) {
            fprintf(stderr, "unable to guess channel layout\n");
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }
        if (avctx->channels == 1)
            is->sdl_channel_layout = AV_CH_LAYOUT_MONO;
        else
            is->sdl_channel_layout = AV_CH_LAYOUT_STEREO;
        is->sdl_channels = av_get_channel_layout_nb_channels(is->sdl_channel_layout);

        wanted_spec.format = AUDIO_S16SYS;
        wanted_spec.freq = is->sdl_sample_rate;
        wanted_spec.channels = is->sdl_channels;
        wanted_spec.silence = 0;
        wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
        wanted_spec.callback = sdl_audio_callback;
        wanted_spec.userdata = is;
        if (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
            fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
            ret = AVERROR_UNKNOWN;
            goto fail;
        }
        is->audio_hw_buf_size = spec.size;
        is->sdl_sample_fmt          = AV_SAMPLE_FMT_S16;
        is->resample_sample_fmt     = is->sdl_sample_fmt;
        is->resample_channel_layout = avctx->channel_layout;
        is->resample_sample_rate    = avctx->sample_rate;
    }

    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    switch (avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        is->audio_stream = stream_index;
        is->audio_st = ic->streams[stream_index];
        is->audio_dec = avctx;
        is->audio_buf_size  = 0;
        is->audio_buf_index = 0;

        /* init averaging filter */
        is->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
        is->audio_diff_avg_count = 0;
        /* since we do not have a precise enough audio FIFO fullness,
           we correct audio sync only if larger than this threshold */
        is->audio_diff_threshold = 2.0 * SDL_AUDIO_BUFFER_SIZE / avctx->sample_rate;

        memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
        packet_queue_init(is, &is->audioq);
        SDL_PauseAudio(0);
        break;
    case AVMEDIA_TYPE_VIDEO:
        is->video_stream = stream_index;
        is->video_st = ic->streams[stream_index];
        is->video_dec = avctx;

        packet_queue_init(is, &is->videoq);
        pthread_create(&is->video_tid, nullptr, video_thread, is);
        break;
    default:
        break;
    }

fail:
    av_dict_free(&opts);

    return ret;
}

static void stream_component_close(PlayerState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecParameters *par;

    if (stream_index < 0 || stream_index >= (int)ic->nb_streams)
        return;
    par = ic->streams[stream_index]->codecpar;

    switch (par->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        packet_queue_abort(&is->audioq);

        SDL_CloseAudio();

        packet_queue_end(&is->audioq);
        av_packet_unref(&is->audio_pkt);
        if (is->avr)
            avresample_free(&is->avr);
        av_freep(&is->audio_buf1);
        is->audio_buf = nullptr;
        av_frame_free(&is->frame);

        if (is->rdft) {
            av_rdft_end(is->rdft);
            av_freep(&is->rdft_data);
            is->rdft = nullptr;
            is->rdft_bits = 0;
        }
        break;
    case AVMEDIA_TYPE_VIDEO:
        packet_queue_abort(&is->videoq);

        /* note: we also signal this mutex to make sure we deblock the
           video thread in all cases */
        pthread_mutex_lock(&is->pictq_mutex);
        pthread_cond_signal(&is->pictq_cond);
        pthread_mutex_unlock(&is->pictq_mutex);

        pthread_join(is->video_tid, nullptr);

        packet_queue_end(&is->videoq);
        break;
    default:
        break;
    }

    ic->streams[stream_index]->discard = AVDISCARD_ALL;
    switch (par->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        avcodec_free_context(&is->audio_dec);
        is->audio_st = nullptr;
        is->audio_stream = -1;
        break;
    case AVMEDIA_TYPE_VIDEO:
        avcodec_free_context(&is->video_dec);
        is->video_st = nullptr;
        is->video_stream = -1;
        break;
    default:
        break;
    }
}

/* since we have only one decoding thread, we can use a global
   variable instead of a thread local variable */
static PlayerState *global_video_state;

static int decode_interrupt_cb(void *ctx)
{
    return global_video_state && global_video_state->abort_request;
}

static void stream_close(PlayerState *is)
{
    /* disable interrupting */
    global_video_state = nullptr;

    /* close each stream */
    if (is->audio_stream >= 0)
        stream_component_close(is, is->audio_stream);
    if (is->video_stream >= 0)
        stream_component_close(is, is->video_stream);
    if (is->ic) {
        avformat_close_input(&is->ic);
    }
}

static int stream_setup(PlayerState *is)
{
    AVFormatContext *ic = nullptr;
    int err, i, ret;
    int st_index[AVMEDIA_TYPE_NB];
    AVDictionaryEntry *t;
    AVDictionary **opts;
    int orig_nb_streams;

    memset(st_index, -1, sizeof(st_index));
    is->video_stream = -1;
    is->audio_stream = -1;

    global_video_state = is;

    ic = avformat_alloc_context();
    if (!ic) {
        av_log(nullptr, AV_LOG_FATAL, "Could not allocate context.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    ic->interrupt_callback.callback = decode_interrupt_cb;

    err = avformat_open_input(&ic, is->filename, is->iformat, &format_opts);
    if (err < 0) {
        // print_error(is->filename, err);
        ret = -1;
        goto fail;
    }

    if ((t = av_dict_get(format_opts, "", nullptr, AV_DICT_IGNORE_SUFFIX))) {
        av_log(nullptr, AV_LOG_ERROR, "Option %s not found.\n", t->key);
        ret = AVERROR_OPTION_NOT_FOUND;
        goto fail;
    }

    is->ic = ic;

    if (is->genpts)
        ic->flags |= AVFMT_FLAG_GENPTS;

    opts = setup_find_stream_info_opts(ic, codec_opts);
    orig_nb_streams = ic->nb_streams;

    for (i = 0; i < (int)ic->nb_streams; i++)
        choose_decoder(is, ic, ic->streams[i]);

    err = avformat_find_stream_info(ic, opts);

    for (i = 0; i < orig_nb_streams; i++)
        av_dict_free(&opts[i]);
    av_freep(&opts);

    if (err < 0) {
        fprintf(stderr, "%s: could not find codec parameters\n", is->filename);
        ret = -1;
        goto fail;
    }

    if (ic->pb)
        ic->pb->eof_reached = 0; // FIXME hack, avplay maybe should not use url_feof() to test for the end

    if (is->seek_by_bytes < 0)
        is->seek_by_bytes = !!(ic->iformat->flags & AVFMT_TS_DISCONT);

    /* if seeking requested, we execute it */
    if (is->start_time != (int64_t)AV_NOPTS_VALUE) {
        int64_t timestamp;

        timestamp = is->start_time;
        /* add the stream start time */
        if (ic->start_time != (int64_t)AV_NOPTS_VALUE)
            timestamp += ic->start_time;
        ret = avformat_seek_file(ic, -1, INT64_MIN, timestamp, INT64_MAX, 0);
        if (ret < 0) {
            fprintf(stderr, "%s: could not seek to position %0.3f\n",
                    is->filename, (double)timestamp / AV_TIME_BASE);
        }
    }

    for (i = 0; i < (int)ic->nb_streams; i++) {
        ic->streams[i]->discard = AVDISCARD_ALL;
    }


    st_index[AVMEDIA_TYPE_VIDEO] =
            av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO,
                                is->wanted_stream[AVMEDIA_TYPE_VIDEO], -1, nullptr, 0);
    st_index[AVMEDIA_TYPE_AUDIO] =
            av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, is->wanted_stream[AVMEDIA_TYPE_AUDIO], st_index[AVMEDIA_TYPE_VIDEO], nullptr, 0);

    // open the streams 
    if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
        stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
    }

    ret = -1;

    if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
        ret = stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO]);
				
        // INFO:: get the fps from media
				AVStream *st = is->ic->streams[st_index[AVMEDIA_TYPE_VIDEO]];

				is->frames_per_second = st->avg_frame_rate.num / (double)st->avg_frame_rate.den;
    }

    if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
        stream_component_open(is, st_index[AVMEDIA_TYPE_SUBTITLE]);
    }

    if (is->video_stream < 0 && is->audio_stream < 0) {
        fprintf(stderr, "%s: could not open codecs\n", is->filename);
        ret = -1;
        goto fail;
    }

    return 0;

fail:
    return ret;
}

/* this thread gets the stream from the disk or the network */
static void * decode_thread(void *arg)
{
    PlayerState *is       = (PlayerState*)arg;
    AVPacket pkt1, *pkt   = &pkt1;
    AVFormatContext *ic   = is->ic;
    int pkt_in_play_range = 0;
    int ret, eof          = 0;

    for (;;) {
        if (is->abort_request)
            break;
        if (is->paused != is->last_paused) {
            is->last_paused = is->paused;
            if (is->paused)
                is->read_pause_return = av_read_pause(ic);
            else
                av_read_play(ic);
        }
#if CONFIG_RTSP_DEMUXER
        if (is->paused && !strcmp(ic->iformat->name, "rtsp")) {
            /* wait 10 ms to avoid trying to get another packet */
            /* XXX: horrible */
            SDL_Delay(10);
            continue;
        }
#endif
        if (is->seek_req) {
            int64_t seek_target = is->seek_pos;
            int64_t seek_min    = is->seek_rel > 0 ? seek_target - is->seek_rel + 2: INT64_MIN;
            int64_t seek_max    = is->seek_rel < 0 ? seek_target - is->seek_rel - 2: INT64_MAX;
// FIXME the +-2 is due to rounding being not done in the correct direction in generation
//      of the seek_pos/seek_rel variables

            ret = avformat_seek_file(is->ic, -1, seek_min, seek_target, seek_max, is->seek_flags);
            if (ret < 0) {
                fprintf(stderr, "error while seeking\n");
                // fprintf(stderr, "%s: error while seeking\n", is->ic->filename);
            } else {
                if (is->audio_stream >= 0) {
                    packet_queue_flush(&is->audioq);
                    packet_queue_put(&is->audioq, &is->flush_pkt);
                }
                if (is->video_stream >= 0) {
                    packet_queue_flush(&is->videoq);
                    packet_queue_put(&is->videoq, &is->flush_pkt);
                }
            }
            is->seek_req = 0;
            eof = 0;
        }

        /* if the queue are full, no need to read more */
        if (!is->infinite_buffer &&
              (is->audioq.size + is->videoq.size > MAX_QUEUE_SIZE
            || (   (is->audioq   .size  > MIN_AUDIOQ_SIZE || is->audio_stream < 0)
                && (is->videoq   .nb_packets > MIN_FRAMES || is->video_stream < 0)))) {
            /* wait 10 ms */
            SDL_Delay(10);
            continue;
        }
        if (eof) {
            if (is->video_stream >= 0) {
                av_init_packet(pkt);
                pkt->data = nullptr;
                pkt->size = 0;
                pkt->stream_index = is->video_stream;
                packet_queue_put(&is->videoq, pkt);
            }
            if (is->audio_stream >= 0 &&
                (is->audio_dec->codec->capabilities & AV_CODEC_CAP_DELAY)) {
                av_init_packet(pkt);
                pkt->data = nullptr;
                pkt->size = 0;
                pkt->stream_index = is->audio_stream;
                packet_queue_put(&is->audioq, pkt);
            }
            SDL_Delay(10);
            if (is->audioq.size + is->videoq.size == 0) {
                if (is->loop != 1 && (!is->loop || --is->loop)) {
                    stream_seek(is, is->start_time != (int64_t)AV_NOPTS_VALUE ? is->start_time : 0, 0, 0);
                } else if (!is->noautoexit) {
                    ret = AVERROR_EOF;
                    goto fail;
                }
            }
            continue;
        }
        ret = av_read_frame(ic, pkt);
        if (ret < 0) {
            if (ret == AVERROR_EOF || (ic->pb && ic->pb->eof_reached))
                eof = 1;
            if (ic->pb && ic->pb->error)
                break;
            SDL_Delay(100); /* wait for user event */
            continue;
        }
        /* check if packet is in play range specified by user, then queue, otherwise discard */
        pkt_in_play_range = is->duration == (int64_t)AV_NOPTS_VALUE ||
                (pkt->pts - ic->streams[pkt->stream_index]->start_time) *
                av_q2d(ic->streams[pkt->stream_index]->time_base) -
                (double)(is->start_time != (int64_t)AV_NOPTS_VALUE ? is->start_time : 0) / 1000000
                <= ((double)is->duration / 1000000);
        if (pkt->stream_index == is->audio_stream && pkt_in_play_range) {
            packet_queue_put(&is->audioq, pkt);
        } else if (pkt->stream_index == is->video_stream && pkt_in_play_range) {
            packet_queue_put(&is->videoq, pkt);
        } else {
            av_packet_unref(pkt);
        }
    }
    /* wait until the end */
    while (!is->abort_request) {
        SDL_Delay(100);
    }

    ret = 0;

fail:
    stream_close(is);

		if (is->endofmedia_callback != nullptr) {
			is->endofmedia_callback(is->endofmedia_callback_data);
		}

    return nullptr;
}

static int stream_open(PlayerState *is,
                       const char *filename, AVInputFormat *iformat)
{
    int ret;

    av_strlcpy(is->filename, filename, sizeof(is->filename));
    is->iformat = iformat;
    is->ytop    = 0;
    is->xleft   = 0;

    if ((ret = stream_setup(is)) < 0) {
        return ret;
    }

    pthread_mutex_init(&is->video_filter_mutex, nullptr);

    /* start video display */
    pthread_mutex_init(&is->pictq_mutex, nullptr);
    pthread_cond_init(&is->pictq_cond, nullptr);

    pthread_mutex_init(&is->subpq_mutex, nullptr);
    pthread_cond_init(&is->subpq_cond, nullptr);

    is->av_sync_type = AV_SYNC_AUDIO_MASTER;
    pthread_create(&is->refresh_tid, nullptr, refresh_thread, is);
    if (!is->refresh_tid)
        return -1;
    pthread_create(&is->parse_tid, nullptr, decode_thread, is);
    if (!is->parse_tid)
        return -1;
    return 0;
}

void avplay_init()
{
	if (avplay_initialized == 1) {
		return;
	}

	avplay_initialized = 1;

  av_log_set_flags(AV_LOG_SKIP_REPEATED);

  // avcodec_register_all(); // CHANGE:: dont need anymore !
  avdevice_register_all();
  // avfilter_register_all(); // CHANGE:: dont need anymore !
  // av_register_all(); // CHANGE:: dont need anymore !
  avformat_network_init();

  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    exit(1);
  }
				 
	// SDL_InitSubSystem(SDL_INIT_AUDIO);
}

void avplay_release()
{
	avformat_network_deinit();

	avplay_initialized = 0;
}

PlayerState *avplay_open(const char *filename)
{
  PlayerState *is = (PlayerState *)malloc(sizeof(struct PlayerState));

  memset(is, 0, sizeof(struct PlayerState));

  av_init_packet(&is->flush_pkt);
  is->flush_pkt.data = (uint8_t *)&is->flush_pkt;

  if (stream_open(is, filename, nullptr) < 0) {
    stream_close(is);

    return nullptr;
  }

  avplay_pause(is, 1);

  is->render_callback = nullptr;
  is->render_callback_data = nullptr;
  is->endofmedia_callback = nullptr;
  is->endofmedia_callback_data = nullptr;

  is->loop = 0;

  is->seek_by_bytes = -1;

  is->start_time = AV_NOPTS_VALUE;
  is->duration = AV_NOPTS_VALUE;
  is->step = 0;
  is->workaround_bugs = 1;
  is->fast = 0;
  is->genpts = 0;
  is->idct = FF_IDCT_AUTO;

  is->skip_frame = AVDISCARD_DEFAULT;
  is->skip_idct = AVDISCARD_DEFAULT;
  is->skip_loop_filter = AVDISCARD_DEFAULT;

  is->error_concealment = 3;
  is->decoder_reorder_pts = -1;
  is->noautoexit = 0;
  is->loop = 1;
  is->framedrop = 1;
  is->infinite_buffer = 0;

  is->rdftspeed = 20;
  is->vfilters = nullptr;
  is->autorotate = 1;

  is->frames_per_second = 0.0;

  is->width = 0;
  is->height = 0;
  is->xleft = 0;
  is->ytop = 0;

  is->wanted_stream[AVMEDIA_TYPE_AUDIO] = -1;
  is->wanted_stream[AVMEDIA_TYPE_VIDEO] = -1;
  is->wanted_stream[AVMEDIA_TYPE_SUBTITLE] = -1;

  std::this_thread::sleep_for(std::chrono::milliseconds((200)));

	return is;
}

void avplay_close(PlayerState *is)
{
	if (is) {
    player_close(is);
		stream_close(is);
	}
}

void avplay_set_rendercallback(PlayerState *is, render_callback_t cb, void *data)
{
	is->render_callback = cb;
	is->render_callback_data = data;
}

void avplay_set_endofmediacallback(PlayerState *is, endofmedia_callback_t cb, void *data)
{
	is->endofmedia_callback = cb;
	is->endofmedia_callback_data = data;
}

void avplay_play(PlayerState *is)
{
	avplay_pause(is, false);
}

void avplay_pause(PlayerState *is, bool state)
{
	is->paused = state;
  is->step = 0;

	if (is->paused) {
    // TODO:: know if the stream is paused
    stream_pause(is);

    /*
		is->frame_timer += av_gettime() / 1000000.0 + is->video_current_pts_drift - is->video_current_pts;
		if (is->read_pause_return != AVERROR(ENOSYS)) {
			is->video_current_pts = is->video_current_pts_drift + av_gettime() / 1000000.0;
		}
		is->video_current_pts_drift = is->video_current_pts - av_gettime() / 1000000.0;
    */
	}
}

void avplay_stop(PlayerState *is)
{
	avplay_setloop(is, false);
	avplay_setcurrentmediatime(is, 0LL);
	avplay_pause(is, true);
}

void avplay_setloop(PlayerState *is, bool state)
{
	if (state == false) {
		is->loop = 0;
	} else {
		is->loop = 1;
	}
}

bool avplay_isloop(PlayerState *is)
{
	return is->loop;
}

void avplay_mute(PlayerState *is, bool state)
{
	if (is == nullptr) {
		return;
	}

	// is->show_audio = state;
}

void avplay_setvolume(PlayerState *is, int level)
{
}

int avplay_getvolume(PlayerState *is)
{
	return 0;
}

int64_t avplay_getmediatime(PlayerState *is)
{
	int64_t t = -1LL;

	if (is && is->ic) {
		if (is->seek_by_bytes || is->ic->duration <= 0) {
			t = avio_size(is->ic->pb);
		} else {
			t = is->ic->duration / 1000LL;
		}
	}

	return t;
}

int64_t avplay_getcurrentmediatime(PlayerState *is)
{
	int64_t t = 0LL;

  if (is && is->seek_by_bytes) {
    if (is->video_stream >= 0 && is->video_current_pos >= 0) {
      t = is->video_current_pos;
    } else if (is->audio_stream >= 0 && is->audio_pkt.pos >= 0) {
      t = is->audio_pkt.pos;
    } else {
      t = avio_tell(is->ic->pb);
    }
  } else {
    t = get_master_clock(is);
  }
	
	return t;
}

void avplay_setcurrentmediatime(PlayerState *is, int64_t time)
{
  int64_t pos = 0;

  if (is && is->seek_by_bytes) {
    if (is->video_stream >= 0 && is->video_current_pos >= 0) {
      pos = is->video_current_pos;
    } else if (is->audio_stream >= 0 && is->audio_pkt.pos >= 0) {
      pos = is->audio_pkt.pos;
    } else {
      pos = avio_tell(is->ic->pb);
    }

    if (is->ic->bit_rate) {
      time *= is->ic->bit_rate / 8.0;
    } else {
      time *= 180000.0;
    }

    pos = pos + time;

    stream_seek(is, pos, time, 1);
  } else {
    pos = get_master_clock(is);

    pos = pos + time;

    stream_seek(is, (int64_t)(pos * AV_TIME_BASE), (int64_t)(time * AV_TIME_BASE), 0);
  }
}

const char * avplay_getmetadata(PlayerState *is, const char *id)
{
	return nullptr;
}

/*
int main(int argc, char **argv)
{
  PlayerState *is = nullptr;

  avplay_init();

  is = avplay_open(argv[1]);

  if (is == nullptr) {
    return -1;
  }

  sleep(5);

  avplay_close(is);
  avplay_release();

  return 0;
}
*/

