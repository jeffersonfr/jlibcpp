#ifndef LIBAVPLAY_H
#define LIBAVPLAY_H

extern "C" {
#include "libavformat/avformat.h"
#include "libavresample/avresample.h"
#include "libavcodec/avfft.h"
#include "libavfilter/avfilter.h"
}

#include <pthread.h>

typedef struct PtsCorrectionContext {
    int64_t num_faulty_pts; /// Number of incorrect PTS values so far
    int64_t num_faulty_dts; /// Number of incorrect DTS values so far
    int64_t last_pts;       /// PTS of the last frame
    int64_t last_dts;       /// DTS of the last frame
} PtsCorrectionContext;

typedef struct SpecifierOpt {
    char *specifier;    /**< stream/chapter/program/... specifier */
    union {
        uint8_t *str;
        int        i;
        int64_t  i64;
        float      f;
        double   dbl;
    } u;
} SpecifierOpt;

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_AUDIOQ_SIZE (20 * 16 * 1024)
#define MIN_FRAMES 5

// SDL audio buffer size, in samples. Should be small to have precise A/V sync as SDL does not have hardware buffer fullness info.
#define SDL_AUDIO_BUFFER_SIZE 1024

// no AV sync correction is done if below the AV sync threshold
#define AV_SYNC_THRESHOLD 0.01
// no AV correction is done if too big error 
#define AV_NOSYNC_THRESHOLD 10.0

#define FRAME_SKIP_FACTOR 0.05

// maximum audio speed change to get correct sync
#define SAMPLE_CORRECTION_PERCENT_MAX 10

// we use about AUDIO_DIFF_AVG_NB A-V differences to make the average 
#define AUDIO_DIFF_AVG_NB   20

// NOTE: the size must be big enough to compensate the hardware audio buffersize size
#define SAMPLE_ARRAY_SIZE (2 * 65536)

typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    int abort_request;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} PacketQueue;

#define VIDEO_PICTURE_QUEUE_SIZE 2
#define SUBPICTURE_QUEUE_SIZE 4

typedef struct VideoPicture {
    double pts;             // presentation timestamp for this picture
    double target_clock;    // av_gettime_relative() time at which this should be displayed ideally
    int64_t pos;            // byte position in file
    uint8_t *bmp[2];						// ARGB[]
    int bmp_index;
    int width, height;      // source height & width
    int allocated;
    int reallocate;
    enum AVPixelFormat pix_fmt;

    AVRational sar;
} VideoPicture;

typedef struct SubPicture {
    double pts;             // presentation time stamp for this picture
    AVSubtitle sub;
} SubPicture;

enum {
    AV_SYNC_AUDIO_MASTER,   // default choice
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, // synchronize to an external clock 
};

typedef void( * render_callback_t)(void *data, uint8_t *buffer, int width, int height);
typedef void( * endofmedia_callback_t)(void *data);

typedef struct PlayerState {
    pthread_t parse_tid;
    pthread_t video_tid;
    pthread_t refresh_tid;
    AVInputFormat *iformat;
    int no_background;
    int abort_request;
    int paused;
    int last_paused;
    int seek_req;
    int seek_flags;
    int64_t seek_pos;
    int64_t seek_rel;
    int read_pause_return;
    AVFormatContext *ic;

    int audio_stream;

    int av_sync_type;
    double external_clock; 
    int64_t external_clock_time;

    double audio_clock;
    double audio_diff_cum; // used for AV difference average computation 
    double audio_diff_avg_coef;
    double audio_diff_threshold;
    int audio_diff_avg_count;
    AVStream *audio_st;
    AVCodecContext *audio_dec;
    PacketQueue audioq;
    int audio_hw_buf_size;
    uint8_t silence_buf[SDL_AUDIO_BUFFER_SIZE];
    uint8_t *audio_buf;
    uint8_t *audio_buf1;
    unsigned int audio_buf_size; /* in bytes */
    int audio_buf_index; /* in bytes */
    AVPacket audio_pkt_temp;
    AVPacket audio_pkt;
    enum AVSampleFormat sdl_sample_fmt;
    uint64_t sdl_channel_layout;
    int sdl_channels;
    int sdl_sample_rate;
    enum AVSampleFormat resample_sample_fmt;
    uint64_t resample_channel_layout;
    int resample_sample_rate;
    AVAudioResampleContext *avr;
    AVFrame *frame;

    int16_t sample_array[SAMPLE_ARRAY_SIZE];
    int sample_array_index;
    int last_i_start;
    RDFTContext *rdft;
    int rdft_bits;
    FFTSample *rdft_data;
    int xpos;

    SubPicture subpq[SUBPICTURE_QUEUE_SIZE];
    int subpq_size, subpq_rindex, subpq_windex;
    pthread_mutex_t subpq_mutex;
    pthread_cond_t subpq_cond;

    double frame_timer;
    double frame_last_pts;
    double frame_last_delay;
    double video_clock;             // pts of last decoded frame / predicted pts of next decoded frame
    int video_stream;
    AVStream *video_st;
    AVCodecContext *video_dec;
    PacketQueue videoq;
    double video_current_pts;       // current displayed pts (different from video_clock if frame fifos are used)
    double video_current_pts_drift; // video_current_pts - time (av_gettime_relative) at which we updated video_current_pts - used to have running video pts
    int64_t video_current_pos;      // current displayed file pos
    VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];
    int pictq_size, pictq_rindex, pictq_windex;
    pthread_mutex_t pictq_mutex;
    pthread_cond_t pictq_cond;

    //    QETimer *video_timer;
    char filename[1024];
    int width, height, xleft, ytop;

    PtsCorrectionContext pts_ctx;

    AVFilterContext *in_video_filter;   // the first filter in the video chain
    AVFilterContext *out_video_filter;  // the last filter in the video chain
    pthread_mutex_t video_filter_mutex;

    float skip_frames;
    float skip_frames_index;
    int refresh;

    SpecifierOpt *codec_names;
    int        nb_codec_names;

    AVPacket flush_pkt;
		
    render_callback_t render_callback;
		void *render_callback_data;
		endofmedia_callback_t endofmedia_callback;
		void *endofmedia_callback_data;
		
    int seek_by_bytes;

    int64_t start_time;
    int64_t duration;
    int step;
    int workaround_bugs;
    int fast;
    int genpts;
    int idct;

    enum AVDiscard skip_frame;
    enum AVDiscard skip_idct;
    enum AVDiscard skip_loop_filter;

    int error_concealment;
    int decoder_reorder_pts1;
    int noautoexit;
    int exit_on_keydown;
    int exit_on_mousedown;
    int loop;
    int framedrop;
    int infinite_buffer;

    int show_audio; // if true, display audio samples

    int rdftspeed;
    char *vfilters;
    int autorotate;

    int64_t audio_callback_time;

    int decoder_reorder_pts;
    
    int wanted_stream[AVMEDIA_TYPE_NB];
 
    float frames_per_second;

    char title[1024];
    char author[1024];
    char album[1024];
    char genre[1024];
    char comments[1024];
    char date[1024];
} PlayerState;

// #########################################################################
// ## Private API ##########################################################
// #########################################################################

void avplay_init();
void avplay_release();
PlayerState *avplay_open(const char *filename);
void avplay_close(PlayerState *is);
void avplay_set_rendercallback(PlayerState *is, render_callback_t cb, void *data);
void avplay_set_endofmediacallback(PlayerState *is, endofmedia_callback_t cb, void *data);
void avplay_play(PlayerState *is);
void avplay_pause(PlayerState *is, bool state);
void avplay_stop(PlayerState *is);
void avplay_setloop(PlayerState *is, bool state);
bool avplay_isloop(PlayerState *is);
void avplay_mute(PlayerState *is, bool state);
void avplay_setvolume(PlayerState *is, int level);
int avplay_getvolume(PlayerState *is);
int64_t avplay_getmediatime(PlayerState *is);
int64_t avplay_getcurrentmediatime(PlayerState *is);
void avplay_setcurrentmediatime(PlayerState *is, int64_t time);
const char * avplay_getmetadata(PlayerState *is, const char *id);

#endif
