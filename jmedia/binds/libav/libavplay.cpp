#include "libavplay.h"

#include <directfb.h>
#include <pthread.h>
#include <SDL2/SDL.h>

const char program_name[] = "avplay";
const int program_birth_year = 2003;

static int packet_queue_put(PacketQueue *q, AVPacket *pkt);

/* packet queue handling */
static void packet_queue_init(PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
		pthread_mutex_init(&q->mutex, NULL);
		pthread_cond_init(&q->cond, NULL); 
    packet_queue_put(q, &q->flush_pkt);
}

static void packet_queue_flush(PacketQueue *q)
{
    AVPacketList *pkt, *pkt1;

    pthread_mutex_lock(&q->mutex);
    for (pkt = q->first_pkt; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = NULL;
    q->first_pkt = NULL;
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

static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    AVPacketList *pkt1;

    /* duplicate the packet */
    if (pkt != &q->flush_pkt && av_dup_packet(pkt) < 0)
        return -1;

    pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
    if (!pkt1)
        return -1;
    pkt1->pkt = *pkt;
    pkt1->next = NULL;


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
                q->last_pkt = NULL;
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

static void video_image_display(VideoState *is)
{
    VideoPicture *vp;
    // SubPicture *sp;
    // AVPicture pict;
    float aspect_ratio;
    int width, height;
		// int x, y;
    // int i;

    vp = &is->pictq[is->pictq_rindex];
    if (vp->bmp) {
#if CONFIG_AVFILTER
         if (!vp->sar.num)
             aspect_ratio = 0;
         else
             aspect_ratio = av_q2d(vp->sar);
#else

        /* XXX: use variable in the frame */
        if (is->video_st->sample_aspect_ratio.num)
            aspect_ratio = av_q2d(is->video_st->sample_aspect_ratio);
        else if (is->video_st->codec->sample_aspect_ratio.num)
            aspect_ratio = av_q2d(is->video_st->codec->sample_aspect_ratio);
        else
            aspect_ratio = 0;
#endif
        if (aspect_ratio <= 0.0)
            aspect_ratio = 1.0;
        aspect_ratio *= (float)vp->width / (float)vp->height;

        /* XXX: we suppose the screen has a 1.0 pixel ratio */
        height = is->height;
        width = ((int)rint(height * aspect_ratio)) & ~1;
        if (width > is->width) {
            width = is->width;
            height = ((int)rint(width / aspect_ratio)) & ~1;
        }
        // x = (is->width - width) / 2;
        // y = (is->height - height) / 2;
        is->no_background = 0;
			
				if (is->render_callback != NULL) {
					is->render_callback(is->render_callback_data, vp->bmp, vp->width, vp->height);
				}
    }
}

/* get the current audio output buffer size, in samples. With SDL, we
   cannot have a precise information */
static int audio_write_get_buf_size(VideoState *is)
{
    return is->audio_buf_size - is->audio_buf_index;
}

static inline int compute_mod(int a, int b)
{
    a = a % b;
    if (a >= 0)
        return a;
    else
        return a + b;
}

/* display the current picture, if any */
static void video_display(VideoState *is)
{
    if (is->video_st)
        video_image_display(is);
}

/* called to display each frame */
static void video_refresh_timer(void *opaque)
{
    VideoState *is = (VideoState*)opaque;
    VideoPicture *vp;

    // SubPicture *sp, *sp2;

    if (is->video_st) {
retry:
        if (is->pictq_size == 0) {
            // nothing to do, no picture to display in the que
        } else {
            double time = av_gettime() / 1000000.0;
            double next_target;
            /* dequeue the picture */
            vp = &is->pictq[is->pictq_rindex];

            if (time < vp->target_clock)
                return;
            /* update current video pts */
            is->video_current_pts = vp->pts;
            is->video_current_pts_drift = is->video_current_pts - time;
            is->video_current_pos = vp->pos;
            if (is->pictq_size > 1) {
                VideoPicture *nextvp = &is->pictq[(is->pictq_rindex + 1) % VIDEO_PICTURE_QUEUE_SIZE];
                // Jeff:: assert(nextvp->target_clock >= vp->target_clock);
                next_target= nextvp->target_clock;
            } else {
                next_target = vp->target_clock + is->video_clock - vp->pts; // FIXME pass durations cleanly
            }
            if (is->framedrop && time > next_target) {
                is->skip_frames *= 1.0 + FRAME_SKIP_FACTOR;
                if (is->pictq_size > 1 || time > next_target + 0.5) {
                    /* update queue size and signal for next picture */
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

            /* update queue size and signal for next picture */
            if (++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE)
                is->pictq_rindex = 0;

            pthread_mutex_lock(&is->pictq_mutex);
            is->pictq_size--;
    				pthread_cond_signal(&is->pictq_cond);
            pthread_mutex_unlock(&is->pictq_mutex);
        }
    } else if (is->audio_st) {
			video_display(is);
		}
}

static void * refresh_thread(void *opaque)
{
		VideoState *is= (VideoState*)opaque;
		while (!is->abort_request) {
			if (is->refresh == 0) {
				video_refresh_timer(is);

				is->refresh = 0;
			}
			av_usleep(5000); // FIXME ideally we should wait the correct time but SDLs event passing is so slow it would be silly
		}
		return NULL;
}

/* get the current audio clock value */
static double get_audio_clock(VideoState *is)
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

/* get the current video clock value */
static double get_video_clock(VideoState *is)
{
    if (is->paused) {
        return is->video_current_pts;
    } else {
        return is->video_current_pts_drift + av_gettime() / 1000000.0;
    }
}

/* get the current external clock value */
static double get_external_clock(VideoState *is)
{
    int64_t ti;
    ti = av_gettime();
    return is->external_clock + ((ti - is->external_clock_time) * 1e-6);
}

/* get the current master clock value */
static double get_master_clock(VideoState *is)
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

/* seek in the stream */
static void stream_seek(VideoState *is, int64_t pos, int64_t rel, int seek_by_bytes)
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

static double compute_target_time(double frame_current_pts, VideoState *is)
{
    double delay, sync_threshold, diff;

    /* compute nominal delay */
    delay = frame_current_pts - is->frame_last_pts;
    if (delay <= 0 || delay >= 10.0) {
        /* if incorrect delay, use previous one */
        delay = is->frame_last_delay;
    } else {
        is->frame_last_delay = delay;
    }
    is->frame_last_pts = frame_current_pts;

    /* update delay to follow master synchronisation source */
    if (((is->av_sync_type == AV_SYNC_AUDIO_MASTER && is->audio_st) ||
         is->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {
        /* if video is slave, we try to correct big delays by
           duplicating or deleting a frame */
        diff = get_video_clock(is) - get_master_clock(is);

        /* skip or repeat frame. We take into account the
           delay to compute the threshold. I still don't know
           if it is the best guess */
        sync_threshold = FFMAX(AV_SYNC_THRESHOLD, delay);
        if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
            if (diff <= -sync_threshold)
                delay = 0;
            else if (diff >= sync_threshold)
                delay = 2 * delay;
        }
    }
    is->frame_timer += delay;

    av_dlog(NULL, "video: delay=%0.3f pts=%0.3f A-V=%f\n",
            delay, frame_current_pts, -diff);

    return is->frame_timer;
}

static void stream_close(VideoState *is)
{
    VideoPicture *vp;
    int i;
    /* XXX: use a special url_shutdown call to abort parse cleanly */
    is->abort_request = 1;
    pthread_join(is->parse_tid, NULL);
    pthread_join(is->refresh_tid, NULL);

    /* free all pictures */
    for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++) {
        vp = &is->pictq[i];
        if (vp->bmp) {
						delete [] vp->bmp;
            vp->bmp = NULL;
        }
    }
		pthread_mutex_destroy(&is->pictq_mutex);
    pthread_cond_destroy(&is->pictq_cond);
		pthread_mutex_destroy(&is->subpq_mutex);
    pthread_cond_destroy(&is->subpq_cond);
#if !CONFIG_AVFILTER
    if (is->img_convert_ctx)
        sws_freeContext(is->img_convert_ctx);
#endif
    av_free(is);
}

/* allocate a picture (needs to do that in main thread to avoid
   potential locking problems */
static void alloc_picture(void *opaque)
{
    VideoState *is = (VideoState*)opaque;
    VideoPicture *vp;

    vp = &is->pictq[is->pictq_windex];

    if (vp->bmp)
				delete [] vp->bmp;

#if CONFIG_AVFILTER
    vp->width   = is->out_video_filter->inputs[0]->w;
    vp->height  = is->out_video_filter->inputs[0]->h;
    vp->pix_fmt = (AVPixelFormat)is->out_video_filter->inputs[0]->format;
#else
    vp->width   = is->video_st->codec->width;
    vp->height  = is->video_st->codec->height;
    vp->pix_fmt = is->video_st->codec->pix_fmt;
#endif
		uint8_t *surface = new uint8_t[4*vp->width*vp->height];

    vp->bmp = surface;

		/* Jeff:: 
    if (!vp->bmp || vp->bmp->pitches[0] < vp->width) {
        do_exit();
    }
		*/

    pthread_mutex_lock(&is->pictq_mutex);
    vp->allocated = 1;
    pthread_cond_signal(&is->pictq_cond);
    pthread_mutex_unlock(&is->pictq_mutex);
}

/* The 'pts' parameter is the dts of the packet / pts of the frame and
 * guessed if not known. */
static int queue_picture(VideoState *is, AVFrame *src_frame, double pts, int64_t pos)
{
    VideoPicture *vp;
		// uint32_t *rgb;
#if CONFIG_AVFILTER
    AVPicture pict_src;
#else
    int dst_pix_fmt = AV_PIX_FMT_YUV420P;
#endif
    /* wait until we have space to put a new picture */
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
    if (!vp->bmp || vp->reallocate ||
#if CONFIG_AVFILTER
        vp->width  != is->out_video_filter->inputs[0]->w ||
        vp->height != is->out_video_filter->inputs[0]->h) {
#else
        vp->width != is->video_st->codec->width ||
        vp->height != is->video_st->codec->height) {
#endif
        vp->allocated  = 0;
        vp->reallocate = 0;

        // Jeff:: the allocation must be done in the main thread to avoid locking problems
				// video_open(is);
				alloc_picture(is);

        /* wait until the picture is allocated */
        pthread_mutex_lock(&is->pictq_mutex);
        while (!vp->allocated && !is->videoq.abort_request) {
        		pthread_cond_wait(&is->pictq_cond, &is->pictq_mutex);
        }
        pthread_mutex_unlock(&is->pictq_mutex);

        if (is->videoq.abort_request)
            return -1;
    }

    /* if the frame is not skipped, then display it */
    if (vp->bmp) {
        AVPicture pict = { { 0 } };

        pict.data[0] = vp->bmp;
        pict.data[1] = NULL; // vp->bmp->pixels[2];
        pict.data[2] = NULL; // vp->bmp->pixels[1];

        pict.linesize[0] = 4*vp->width;
        pict.linesize[1] = 0; // vp->bmp->pitches[2];
        pict.linesize[2] = 0; // vp->bmp->pitches[1];

#if CONFIG_AVFILTER
        pict_src.data[0] = src_frame->data[0];
        pict_src.data[1] = src_frame->data[1];
        pict_src.data[2] = src_frame->data[2];

        pict_src.linesize[0] = src_frame->linesize[0];
        pict_src.linesize[1] = src_frame->linesize[1];
        pict_src.linesize[2] = src_frame->linesize[2];

        // FIXME use direct rendering
        av_picture_copy(&pict, &pict_src,
                        vp->pix_fmt, vp->width, vp->height);
#else
        av_opt_get_int(sws_opts, "sws_flags", 0, &sws_flags);
        is->img_convert_ctx = sws_getCachedContext(is->img_convert_ctx,
            vp->width, vp->height, vp->pix_fmt, vp->width, vp->height,
            dst_pix_fmt, sws_flags, NULL, NULL, NULL);
        if (!is->img_convert_ctx) {
            fprintf(stderr, "Cannot initialize the conversion context\n");
            exit(1);
        }
        sws_scale(is->img_convert_ctx, src_frame->data, src_frame->linesize,
                  0, vp->height, pict.data, pict.linesize);
#endif
        /* update the bitmap content */
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
static int output_picture2(VideoState *is, AVFrame *src_frame, double pts1, int64_t pos)
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
    frame_delay = av_q2d(is->video_st->codec->time_base);
    /* for MPEG2, the frame can be repeated, so we update the
       clock accordingly */
    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
    is->video_clock += frame_delay;

    ret = queue_picture(is, src_frame, pts, pos);
    av_frame_unref(src_frame);
    return ret;
}

static int get_video_frame(VideoState *is, AVFrame *frame, int64_t *pts, AVPacket *pkt)
{
    int got_picture, i;

    if (packet_queue_get(&is->videoq, pkt, 1) < 0)
        return -1;

    if (pkt->data == is->videoq.flush_pkt.data) {
        avcodec_flush_buffers(is->video_st->codec);

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
        is->frame_timer = (double)av_gettime() / 1000000.0;
        is->skip_frames = 1;
        is->skip_frames_index = 0;
        return 0;
    }

    avcodec_decode_video2(is->video_st->codec, frame, &got_picture, pkt);

    if (got_picture) {
				// Jeff::
				int decoder_reorder_pts = -1;
        if (decoder_reorder_pts == -1) {
            *pts = guess_correct_pts(&is->pts_ctx, frame->pkt_pts, frame->pkt_dts);
        } else if (decoder_reorder_pts) {
            *pts = frame->pkt_pts;
        } else {
            *pts = frame->pkt_dts;
        }

        if (*pts == AV_NOPTS_VALUE) {
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

#if CONFIG_AVFILTER
static int configure_video_filters(AVFilterGraph *graph, VideoState *is, const char *vfilters)
{
    char sws_flags_str[128];
    char buffersrc_args[256];
    int ret;
    AVFilterContext *filt_src = NULL, *filt_out = NULL, *filt_format;
    AVCodecContext *codec = is->video_st->codec;

    snprintf(sws_flags_str, sizeof(sws_flags_str), "flags=%"PRId64, is->sws_flags);
    graph->scale_sws_opts = av_strdup(sws_flags_str);

    snprintf(buffersrc_args, sizeof(buffersrc_args), "%d:%d:%d:%d:%d:%d:%d",
             codec->width, codec->height, codec->pix_fmt,
             is->video_st->time_base.num, is->video_st->time_base.den,
             codec->sample_aspect_ratio.num, codec->sample_aspect_ratio.den);


    if ((ret = avfilter_graph_create_filter(&filt_src,
                                            avfilter_get_by_name("buffer"),
                                            "src", buffersrc_args, NULL,
                                            graph)) < 0)
        return ret;
    if ((ret = avfilter_graph_create_filter(&filt_out,
                                            avfilter_get_by_name("buffersink"),
                                            "out", NULL, NULL, graph)) < 0)
        return ret;

    if ((ret = avfilter_graph_create_filter(&filt_format,
                                            avfilter_get_by_name("format"),
                                            "format", "bgra", NULL, graph)) < 0)
        return ret;
    if ((ret = avfilter_link(filt_format, 0, filt_out, 0)) < 0)
        return ret;


    if (vfilters) {
        AVFilterInOut *outputs = avfilter_inout_alloc();
        AVFilterInOut *inputs  = avfilter_inout_alloc();

        outputs->name    = av_strdup("in");
        outputs->filter_ctx = filt_src;
        outputs->pad_idx = 0;
        outputs->next    = NULL;

        inputs->name    = av_strdup("out");
        inputs->filter_ctx = filt_format;
        inputs->pad_idx = 0;
        inputs->next    = NULL;

        if ((ret = avfilter_graph_parse(graph, vfilters, inputs, outputs, NULL)) < 0)
            return ret;
    } else {
        if ((ret = avfilter_link(filt_src, 0, filt_format, 0)) < 0)
            return ret;
    }

    if ((ret = avfilter_graph_config(graph, NULL)) < 0)
        return ret;

    is->in_video_filter  = filt_src;
    is->out_video_filter = filt_out;

    return ret;
}

#endif  /* CONFIG_AVFILTER */

static void * video_thread(void *arg)
{
    AVPacket pkt = { 0 };
    VideoState *is = (VideoState*)arg;
    AVFrame *frame = av_frame_alloc();
    int64_t pts_int;
    double pts;
    int ret;

#if CONFIG_AVFILTER
    AVFilterGraph *graph = avfilter_graph_alloc();
    AVFilterContext *filt_out = NULL, *filt_in = NULL;
    int last_w = is->video_st->codec->width;
    int last_h = is->video_st->codec->height;

    if ((ret = configure_video_filters(graph, is, is->vfilters)) < 0)
        goto the_end;
    filt_in  = is->in_video_filter;
    filt_out = is->out_video_filter;
#endif

    for (;;) {
#if CONFIG_AVFILTER
        AVRational tb;
#endif
        while (is->paused && !is->videoq.abort_request)
            usleep(10000);

        av_free_packet(&pkt);

        ret = get_video_frame(is, frame, &pts_int, &pkt);
        if (ret < 0)
            goto the_end;

        if (!ret)
            continue;

#if CONFIG_AVFILTER
        if (   last_w != is->video_st->codec->width
            || last_h != is->video_st->codec->height) {
            av_dlog(NULL, "Changing size %dx%d -> %dx%d\n", last_w, last_h,
                    is->video_st->codec->width, is->video_st->codec->height);
            avfilter_graph_free(&graph);
            graph = avfilter_graph_alloc();
            if ((ret = configure_video_filters(graph, is, is->vfilters)) < 0)
                goto the_end;
            filt_in  = is->in_video_filter;
            filt_out = is->out_video_filter;
            last_w = is->video_st->codec->width;
            last_h = is->video_st->codec->height;
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
                av_dlog(NULL, "video_thread(): "
                        "tb:%d/%d pts:%"PRId64" -> tb:%d/%d pts:%"PRId64"\n",
                        tb.num, tb.den, pts1,
                        is->video_st->time_base.num, is->video_st->time_base.den, pts_int);
            }
            pts = pts_int * av_q2d(is->video_st->time_base);
            ret = output_picture2(is, frame, pts, 0);
        }
#else
        pts = pts_int * av_q2d(is->video_st->time_base);
        ret = output_picture2(is, frame, pts,  pkt.pos);
#endif

        if (ret < 0)
            goto the_end;
    }
 the_end:
#if CONFIG_AVFILTER
    av_freep(&is->vfilters);
    avfilter_graph_free(&graph);
#endif
    av_free_packet(&pkt);
    av_frame_free(&frame);
    return NULL;
}

/* copy samples for viewing in editor window */
static void update_sample_display(VideoState *is, short *samples, int samples_size)
{
    int size, len;

    size = samples_size / sizeof(short);
    while (size > 0) {
        len = SAMPLE_ARRAY_SIZE - is->sample_array_index;
        if (len > size)
            len = size;
        memcpy(is->sample_array + is->sample_array_index, samples, len * sizeof(short));
        samples += len;
        is->sample_array_index += len;
        if (is->sample_array_index >= SAMPLE_ARRAY_SIZE)
            is->sample_array_index = 0;
        size -= len;
    }
}

/* return the new audio buffer size (samples can be added or deleted
   to get better sync if video or external master clock) */
static int synchronize_audio(VideoState *is, short *samples,
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
                av_dlog(NULL, "diff=%f adiff=%f sample_diff=%d apts=%0.3f vpts=%0.3f %f\n",
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
static int audio_decode_frame(VideoState *is, double *pts_ptr)
{
    AVPacket *pkt_temp = &is->audio_pkt_temp;
    AVPacket *pkt = &is->audio_pkt;
    AVCodecContext *dec = is->audio_st->codec;
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
                if (!pkt_temp->data && dec->codec->capabilities & CODEC_CAP_DELAY)
                    flush_complete = 1;
                continue;
            }
            data_size = av_samples_get_buffer_size(NULL, dec->channels,
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
            av_free_packet(pkt);
        memset(pkt_temp, 0, sizeof(*pkt_temp));

        if (is->paused || is->audioq.abort_request) {
            return -1;
        }

        /* read next packet */
        if ((new_packet = packet_queue_get(&is->audioq, pkt, 1)) < 0)
            return -1;

        if (pkt->data == is->audioq.flush_pkt.data) {
            avcodec_flush_buffers(dec);
            flush_complete = 0;
        }

        *pkt_temp = *pkt;

        /* if update the audio clock with the pts */
        if (pkt->pts != AV_NOPTS_VALUE) {
            is->audio_clock = av_q2d(is->audio_st->time_base)*pkt->pts;
        }
    }
}

/* prepare a new audio buffer */
static void sdl_audio_callback(void *opaque, uint8_t *stream, int len)
{
    VideoState *is = (VideoState*)opaque;
    int audio_size, len1;
    double pts;

    while (len > 0) {
        if (is->audio_buf_index >= (int)is->audio_buf_size) {
           audio_size = audio_decode_frame(is, &pts);
           if (audio_size < 0) {
                /* if error, just output silence */
               is->audio_buf      = is->silence_buf;
               is->audio_buf_size = sizeof(is->silence_buf);
           } else {
               if (is->show_audio)
                   update_sample_display(is, (int16_t *)is->audio_buf, audio_size);
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

/* open a given stream. Return 0 if OK */
static int stream_component_open(VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;
    AVCodec *codec;
    SDL_AudioSpec wanted_spec, spec;
    AVDictionary *opts;
    AVDictionaryEntry *t = NULL;

    if (stream_index < 0 || stream_index >= (int)ic->nb_streams)
        return -1;
    avctx = ic->streams[stream_index]->codec;

    opts = filter_codec_opts(codec_opts, avctx->codec_id, ic, ic->streams[stream_index], NULL);

    codec = avcodec_find_decoder(avctx->codec_id);
    avctx->workaround_bugs   = 1;
    avctx->idct_algo         = FF_IDCT_AUTO;
    avctx->skip_frame        = AVDISCARD_DEFAULT;
    avctx->skip_idct         = AVDISCARD_DEFAULT;
    avctx->skip_loop_filter  = AVDISCARD_DEFAULT;
    avctx->error_concealment = 3;

		// Jeff::
    // avctx->flags2 |= CODEC_FLAG2_FAST;

    if (!av_dict_get(opts, "threads", NULL, 0))
        av_dict_set(&opts, "threads", "auto", 0);
    if (avctx->codec_type == AVMEDIA_TYPE_VIDEO)
        av_dict_set(&opts, "refcounted_frames", "1", 0);
    if (!codec ||
        avcodec_open2(avctx, codec, &opts) < 0)
        return -1;
    if ((t = av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
        av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
        return AVERROR_OPTION_NOT_FOUND;
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
            return -1;
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
        wanted_spec.samples = ALSA_AUDIO_BUFFER_SIZE;
        wanted_spec.callback = sdl_audio_callback;
        wanted_spec.userdata = is;
				if (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
					fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());

					return -1;
				}
				is->audio_hw_buf_size = spec.size;
        is->audio_hw_buf_size = 1024*is->sdl_channels*ALSA_AUDIO_BUFFER_SIZE*2;
        is->sdl_sample_fmt          = AV_SAMPLE_FMT_S16;
        is->resample_sample_fmt     = is->sdl_sample_fmt;
        is->resample_channel_layout = avctx->channel_layout;
        is->resample_sample_rate    = avctx->sample_rate;
    }

    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    switch (avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
				is->has_audio = true;
        is->audio_stream = stream_index;
        is->audio_st = ic->streams[stream_index];
        is->audio_buf_size  = 0;
        is->audio_buf_index = 0;

        /* init averaging filter */
        is->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
        is->audio_diff_avg_count = 0;
        /* since we do not have a precise anough audio fifo fullness,
           we correct audio sync only if larger than this threshold */
        is->audio_diff_threshold = 2.0 * ALSA_AUDIO_BUFFER_SIZE / avctx->sample_rate;

        memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
        packet_queue_init(&is->audioq);
        SDL_PauseAudio(0);
        break;
    case AVMEDIA_TYPE_VIDEO:
				is->has_video = true;
        is->video_stream = stream_index;
        is->video_st = ic->streams[stream_index];

        packet_queue_init(&is->videoq);
        pthread_create(&is->video_tid, NULL, video_thread, is);
        break;
    default:
        break;
    }
    return 0;
}

static void stream_component_close(VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;

    if (stream_index < 0 || stream_index >= (int)ic->nb_streams)
        return;
    avctx = ic->streams[stream_index]->codec;

    switch (avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        packet_queue_abort(&is->audioq);

       	SDL_CloseAudio();

        packet_queue_end(&is->audioq);
        av_free_packet(&is->audio_pkt);
        if (is->avr)
            avresample_free(&is->avr);
        av_freep(&is->audio_buf1);
        is->audio_buf = NULL;
        av_frame_free(&is->frame);

        if (is->rdft) {
            av_rdft_end(is->rdft);
            av_freep(&is->rdft_data);
            is->rdft = NULL;
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

        pthread_join(is->video_tid, NULL);

        packet_queue_end(&is->videoq);
        break;
    default:
        break;
    }

    ic->streams[stream_index]->discard = AVDISCARD_ALL;
    avcodec_close(avctx);
    switch (avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        is->audio_st = NULL;
        is->audio_stream = -1;
        break;
    case AVMEDIA_TYPE_VIDEO:
        is->video_st = NULL;
        is->video_stream = -1;
        break;
    default:
        break;
    }
}

/* since we have only one decoding thread, we can use a global
   variable instead of a thread local variable */
static VideoState *global_video_state;

static int decode_interrupt_cb(void *ctx)
{
    return global_video_state && global_video_state->abort_request;
}

/* this thread gets the stream from the disk or the network */
static void * decode_thread(void *arg)
{
    VideoState *is = (VideoState*)arg;
    AVFormatContext *ic = NULL;
    int err, i, ret;
    int st_index[AVMEDIA_TYPE_NB];
    AVPacket pkt1, *pkt = &pkt1;
    int eof = 0;
    int pkt_in_play_range = 0;
    AVDictionaryEntry *t;
    AVDictionary **opts;
    int orig_nb_streams;

    memset(st_index, -1, sizeof(st_index));
    is->video_stream = -1;
    is->audio_stream = -1;

    global_video_state = is;

    ic = avformat_alloc_context();
    ic->interrupt_callback.callback = decode_interrupt_cb;
    err = avformat_open_input(&ic, is->filename, is->iformat, &format_opts);
    if (err < 0) {
        print_error(is->filename, err);
        ret = -1;
        goto fail;
    }
    if ((t = av_dict_get(format_opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
        av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
        ret = AVERROR_OPTION_NOT_FOUND;
        goto fail;
    }
    is->ic = ic;

		// Jeff::
    // ic->flags |= AVFMT_FLAG_GENPTS;

    opts = setup_find_stream_info_opts(ic, codec_opts);
    orig_nb_streams = ic->nb_streams;

    err = avformat_find_stream_info(ic, opts);
    if (err < 0) {
        fprintf(stderr, "%s: could not find codec parameters\n", is->filename);
        ret = -1;
        goto fail;
    }
    for (i = 0; i < orig_nb_streams; i++)
        av_dict_free(&opts[i]);
    av_freep(&opts);

    if (ic->pb)
        ic->pb->eof_reached = 0; // FIXME hack, avplay maybe should not use url_feof() to test for the end

    if (is->seek_by_bytes < 0)
        is->seek_by_bytes = !!(ic->iformat->flags & AVFMT_TS_DISCONT);

    /* if seeking requested, we execute it */
    if (is->start_time != AV_NOPTS_VALUE) {
        int64_t timestamp;

        timestamp = is->start_time;
        /* add the stream start time */
        if (ic->start_time != AV_NOPTS_VALUE)
            timestamp += ic->start_time;
        ret = avformat_seek_file(ic, -1, INT64_MIN, timestamp, INT64_MAX, 0);
        if (ret < 0) {
            fprintf(stderr, "%s: could not seek to position %0.3f\n",
                    is->filename, (double)timestamp / AV_TIME_BASE);
        }
    }

    for (i = 0; i < (int)ic->nb_streams; i++)
        ic->streams[i]->discard = AVDISCARD_ALL;
    
    st_index[AVMEDIA_TYPE_VIDEO] =
        av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO,
                            -1, -1, NULL, 0);
                            // wanted_stream[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);
    
		st_index[AVMEDIA_TYPE_AUDIO] =
        av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO,
                            -1,
                            // wanted_stream[AVMEDIA_TYPE_AUDIO],
                            st_index[AVMEDIA_TYPE_VIDEO],
                            NULL, 0);

    /* open the streams */
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
    pthread_create(&is->refresh_tid, NULL, refresh_thread, is);
    if (ret < 0) {
			is->show_audio = 2;
		}

    if (is->video_stream < 0 && is->audio_stream < 0) {
        fprintf(stderr, "%s: could not open codecs\n", is->filename);
        ret = -1;
        goto fail;
    }

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
            usleep(10000);
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
                fprintf(stderr, "%s: error while seeking\n", is->ic->filename);
            } else {
                if (is->audio_stream >= 0) {
                    packet_queue_flush(&is->audioq);
                    packet_queue_put(&is->audioq, &is->audioq.flush_pkt);
                }
                if (is->video_stream >= 0) {
                    packet_queue_flush(&is->videoq);
                    packet_queue_put(&is->videoq, &is->videoq.flush_pkt);
                }
            }
            is->seek_req = 0;
            eof = 0;
        }

        /* if the queue are full, no need to read more */
        if (!is->infinite_buffer &&
              (is->audioq.size + is->videoq.size > MAX_QUEUE_SIZE
            || (   (is->audioq   .size  > MIN_AUDIOQ_SIZE || is->audio_stream < 0)
                && (is->videoq   .nb_packets > MIN_FRAMES || is->video_stream < 0)
                ))) {
            /* wait 10 ms */
            usleep(10000);
            continue;
        }
        if (eof) {
            if (is->video_stream >= 0) {
                av_init_packet(pkt);
                pkt->data = NULL;
                pkt->size = 0;
                pkt->stream_index = is->video_stream;
                packet_queue_put(&is->videoq, pkt);
            }
            if (is->audio_stream >= 0 &&
                is->audio_st->codec->codec->capabilities & CODEC_CAP_DELAY) {
                av_init_packet(pkt);
                pkt->data = NULL;
                pkt->size = 0;
                pkt->stream_index = is->audio_stream;
                packet_queue_put(&is->audioq, pkt);
            }
            usleep(10000);
            if (is->audioq.size + is->videoq.size == 0) {
                if (is->loop != 0) {
                    stream_seek(is, is->start_time != AV_NOPTS_VALUE ? is->start_time : 0, 0, 0);
                } else {
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
            usleep(10000);
            continue;
        }
        /* check if packet is in play range specified by user, then queue, otherwise discard */
        pkt_in_play_range = is->duration == AV_NOPTS_VALUE ||
                (pkt->pts - ic->streams[pkt->stream_index]->start_time) *
                av_q2d(ic->streams[pkt->stream_index]->time_base) -
                (double)(is->start_time != AV_NOPTS_VALUE ? is->start_time : 0) / 1000000
                <= ((double)is->duration / 1000000);
        if (pkt->stream_index == is->audio_stream && pkt_in_play_range) {
            packet_queue_put(&is->audioq, pkt);
        } else if (pkt->stream_index == is->video_stream && pkt_in_play_range) {
            packet_queue_put(&is->videoq, pkt);
        } else {
            av_free_packet(pkt);
        }
    }
    /* wait until the end */
    while (!is->abort_request) {
				usleep(100000);
		}

    ret = 0;
 fail:
    /* disable interrupting */
    global_video_state = NULL;

    /* close each stream */
    if (is->audio_stream >= 0)
        stream_component_close(is, is->audio_stream);
    if (is->video_stream >= 0)
        stream_component_close(is, is->video_stream);
    if (is->ic) {
        avformat_close_input(&is->ic);
    }

		if (is->endofmedia_callback != NULL) {
			is->endofmedia_callback(is->endofmedia_callback_data);
		}
    
		return NULL;
}

static VideoState *stream_open(const char *filename, AVInputFormat *iformat)
{
    VideoState *is;

    is = (VideoState*)av_mallocz(sizeof(VideoState));
    if (!is)
        return NULL;
    strcpy(is->filename, filename);
    is->iformat = iformat;
    is->ytop    = 0;
    is->xleft   = 0;

    /* start video display */
		pthread_mutex_init(&is->pictq_mutex, NULL);
		pthread_cond_init(&is->pictq_cond, NULL);

		pthread_mutex_init(&is->subpq_mutex, NULL);
		pthread_cond_init(&is->subpq_cond, NULL);

    is->av_sync_type = is->av_sync_type;
    if (pthread_create(&is->parse_tid, NULL, decode_thread, is) != 0) {
        av_free(is);
        return NULL;
    }
    return is;
}

static const OptionDef options[] = {
    { NULL }
};

void show_help_default(const char *opt, const char *arg)
{
}

// #########################################################################
// ## Private API ##########################################################
// #########################################################################
static bool avplay_initialized = false;

void avplay_init()
{
	if (avplay_initialized == true) {
		return;
	}

	avplay_initialized = true;

	av_log_set_flags(AV_LOG_SKIP_REPEATED);

	/* register all codecs, demux and protocols */
	avcodec_register_all();
#if CONFIG_AVFILTER
	avfilter_register_all();
#endif
	av_register_all();
	avformat_network_init();

	init_opts();
}

void avplay_release()
{
	uninit_opts();
	avformat_network_deinit();

	avplay_initialized = false;
}

VideoState *avplay_open(const char *filename)
{
	VideoState *is = stream_open(filename, NULL);

	if (is != NULL) {
		av_init_packet(&is->audioq.flush_pkt);
		av_init_packet(&is->videoq.flush_pkt);

		is->audioq.flush_pkt.data = (uint8_t *)&is->audioq.flush_pkt;
		is->videoq.flush_pkt.data = (uint8_t *)&is->videoq.flush_pkt;
	
    is->iformat = NULL;
    is->no_background = 0;
    is->abort_request = 0;
    is->paused = 0;
    is->last_paused = 0;
    is->seek_req = 0;
    is->seek_flags = 0;
		is->loop = 0;
    is->seek_pos = 0;
    is->seek_rel = 0;
    is->read_pause_return = 0;
    is->ic = NULL;

		is->render_callback = NULL;
		is->render_callback_data = NULL;
		is->endofmedia_callback = NULL;
		is->endofmedia_callback_data = NULL;

		is->sws_flags = SWS_BICUBIC;
		is->seek_by_bytes = -1;
		is->av_sync_type = AV_SYNC_AUDIO_MASTER;
		is->start_time = AV_NOPTS_VALUE;
		is->duration = AV_NOPTS_VALUE;
		is->framedrop = 1;
		is->infinite_buffer = 0;

#if CONFIG_AVFILTER
		is->vfilters = NULL;
#endif

    is->audio_stream = 0;
    is->external_clock = 0.0;
    is->external_clock_time = 0LL;
    is->audio_clock = 0.0;
    is->audio_diff_cum = 0.0;
    is->audio_diff_avg_coef = 0.0;
    is->audio_diff_threshold = 0.0;
    is->audio_diff_avg_count = 0;
    is->audio_st = NULL;
    is->audio_hw_buf_size = 0;
    is->audio_buf = 0;
    is->audio_buf1 = 0;
    is->audio_buf_size = 0;
    is->audio_buf_index = 0;

    is->sdl_channel_layout = 0;
    is->sdl_channels = 0;
    is->sdl_sample_rate = 0;
    is->resample_channel_layout = 0LL;
    is->resample_sample_rate = 0;
    is->avr = NULL;
    is->frame = NULL;

    is->show_audio = 0;
    is->sample_array_index = 0;
    is->last_i_start = 0;
    is->rdft = NULL;
    is->rdft_bits = 0;
    is->rdft_data = NULL;
    is->xpos = 0;

    is->subpq_size = 0;
		is->subpq_rindex = 0;
		is->subpq_windex = 0;

    is->frame_timer = 0.0;
    is->frame_last_pts = 0.0;
    is->frame_last_delay = 0.0;
    is->video_clock = 0.0;
    is->video_stream = 0;
    is->video_st = NULL;
    is->video_current_pts = 0.0;
    is->video_current_pts_drift = 0.0;
    is->video_current_pos = 0LL;
    is->pictq_size = 0;
		is->pictq_rindex = 0;
		is->pictq_windex = 0;

#if !CONFIG_AVFILTER
    is->img_convert_ctx = NULL;
#endif

		is->frames_per_second = 0.0;

    is->width = 0;
		is->height = 0;
		is->xleft = 0;
		is->ytop = 0;

#if CONFIG_AVFILTER
    is->in_video_filter = 0;
    is->out_video_filter = 0;
#endif

    is->skip_frames = 0.0;
    is->skip_frames_index = 0.0;
    is->refresh = 0;

		avplay_pause(is, true);
		
		// usleep(100000);
	}

	return is;
}

void avplay_close(VideoState *is)
{
	if (is) {
		stream_close(is);
	}
}

void avplay_set_rendercallback(VideoState *is, render_callback_t cb, void *data)
{
	is->render_callback = cb;
	is->render_callback_data = data;
}

void avplay_set_endofmediacallback(VideoState *is, endofmedia_callback_t cb, void *data)
{
	is->endofmedia_callback = cb;
	is->endofmedia_callback_data = data;
}

void avplay_play(VideoState *is)
{
	avplay_pause(is, false);
}

void avplay_pause(VideoState *is, bool state)
{
	is->paused = state;

	if (is->paused) {
		is->frame_timer += av_gettime() / 1000000.0 + is->video_current_pts_drift - is->video_current_pts;
		if (is->read_pause_return != AVERROR(ENOSYS)) {
			is->video_current_pts = is->video_current_pts_drift + av_gettime() / 1000000.0;
		}
		is->video_current_pts_drift = is->video_current_pts - av_gettime() / 1000000.0;
	}
}

void avplay_stop(VideoState *is)
{
	avplay_setloop(is, false);
	avplay_setcurrentmediatime(is, 0LL);
	avplay_pause(is, true);
}

void avplay_setloop(VideoState *is, bool state)
{
	if (state == false) {
		is->loop = 0;
	} else {
		is->loop = 1;
	}
}

bool avplay_isloop(VideoState *is)
{
	return is->loop;
}

void avplay_mute(VideoState *is, bool state)
{
	if (is == NULL) {
		return;
	}

	is->show_audio = state;
}

void avplay_setvolume(VideoState *is, int level)
{
}

int avplay_getvolume(VideoState *is)
{
	return 0;
}

int64_t avplay_getmediatime(VideoState *is)
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

int64_t avplay_getcurrentmediatime(VideoState *is)
{
	int64_t t = 0LL;

	if (is && is->ic) {
		if (is->seek_by_bytes) {
			if (is->video_stream >= 0 && is->video_current_pos >= 0) {
				t = is->video_current_pos;
			} else if (is->audio_stream >= 0 && is->audio_pkt.pos >= 0) {
				t = is->audio_pkt.pos;
			} else {
				t = avio_tell(is->ic->pb);
			}
		}
	}
	
	return t;
}

void avplay_setcurrentmediatime(VideoState *is, int64_t time)
{
	if (is && is->ic) {
		if (is->seek_by_bytes || is->ic->duration <= 0) {
			uint64_t size =  avio_size(is->ic->pb);

			stream_seek(is, size*time/is->width, 0, 1);
		} else {
			int frac = 0;
			
			if (is->width > 0) {
				frac = time / is->width;
			}

			int64_t ts = frac * is->ic->duration;
			
			if (is->ic->start_time != AV_NOPTS_VALUE) {
				ts += is->ic->start_time;
			}
			
			stream_seek(is, ts, 0, 0);
		}
	}
}

const char * avplay_getmetadata(VideoState *is, const char *id)
{
	return NULL;
}

