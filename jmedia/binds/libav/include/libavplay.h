#define CONFIG_AVCODEC 1
#define CONFIG_AVDEVICE 1
#define CONFIG_AVFILTER 1
#define CONFIG_AVFORMAT 1
#define CONFIG_AVRESAMPLE 1
#define CONFIG_AVUTIL 1
#define CONFIG_SWSCALE 1
#define CONFIG_RTSP_DEMUXER 1

#include <string>

extern "C" {
#include "libavutil/time.h"
#include "libavresample/avresample.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"

#if CONFIG_AVFILTER
# include "libavfilter/buffersink.h"
# include "libavfilter/buffersrc.h"
#endif

#include "libavutils.h"
}

#include <SDL2/SDL.h>

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_AUDIOQ_SIZE (20 * 16 * 1024)
#define MIN_FRAMES 5

#define ALSA_AUDIO_BUFFER_SIZE 1024

/* no AV sync correction is done if below the AV sync threshold */
#define AV_SYNC_THRESHOLD 0.01
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

#define FRAME_SKIP_FACTOR 0.05

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
#define SAMPLE_ARRAY_SIZE (2 * 65536)

typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    int abort_request;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
		AVPacket flush_pkt;
} PacketQueue;

#define VIDEO_PICTURE_QUEUE_SIZE 2
#define SUBPICTURE_QUEUE_SIZE 4

typedef struct VideoPicture {
    double pts;             // presentation timestamp for this picture
    double target_clock;    // av_gettime() time at which this should be displayed ideally
    int64_t pos;            // byte position in file
    uint8_t *bmp;						// ARGB[]
    int width, height; /* source height & width */
    int allocated;
    int reallocate;
    enum AVPixelFormat pix_fmt;

    AVRational sar;
} VideoPicture;

typedef struct SubPicture {
    double pts; /* presentation time stamp for this picture */
    AVSubtitle sub;
} SubPicture;

enum {
    AV_SYNC_AUDIO_MASTER, /* default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

typedef void( * render_callback_t)(void *data, uint8_t *buffer, int width, int height);
typedef void( * endofmedia_callback_t)(void *data);

typedef struct VideoState {
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
		int loop;
    int64_t seek_pos;
    int64_t seek_rel;
    int read_pause_return;
    AVFormatContext *ic;

		render_callback_t render_callback;
		void *render_callback_data;
		endofmedia_callback_t endofmedia_callback;
		void *endofmedia_callback_data;

		int64_t sws_flags;

		/* options specified by the user */
		int seek_by_bytes;
		int av_sync_type;
		int64_t start_time;
		int64_t duration;
		int framedrop;
		int infinite_buffer;

#if CONFIG_AVFILTER
		char *vfilters;
#endif

		SDL_AudioDeviceID audio_device;

    int audio_stream;

    double external_clock; /* external clock base */
    int64_t external_clock_time;

    double audio_clock;
    double audio_diff_cum; /* used for AV difference average computation */
    double audio_diff_avg_coef;
    double audio_diff_threshold;
    int audio_diff_avg_count;
    AVStream *audio_st;
    PacketQueue audioq;
    int audio_hw_buf_size;
    uint8_t silence_buf[ALSA_AUDIO_BUFFER_SIZE];
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

    int show_audio; /* if true, display audio samples */
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
    PacketQueue videoq;
    double video_current_pts;       // current displayed pts (different from video_clock if frame fifos are used)
    double video_current_pts_drift; // video_current_pts - time (av_gettime) at which we updated video_current_pts - used to have running video pts
    int64_t video_current_pos;      // current displayed file pos
    VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];
    int pictq_size, pictq_rindex, pictq_windex;
    pthread_mutex_t pictq_mutex;
    pthread_cond_t pictq_cond;
#if !CONFIG_AVFILTER
    struct SwsContext *img_convert_ctx;
#endif

    //    QETimer *video_timer;
    char filename[1024];
    int width, height, xleft, ytop;

    PtsCorrectionContext pts_ctx;

#if CONFIG_AVFILTER
    AVFilterContext *in_video_filter;   // the first filter in the video chain
    AVFilterContext *out_video_filter;  // the last filter in the video chain
#endif

		double frames_per_second;

    float skip_frames;
    float skip_frames_index;
    int refresh;

		bool has_audio;
		bool has_video;

    char title[1024];
    char author[1024];
    char album[1024];
    char genre[1024];
    char comments[1024];
    char date[1024];
} VideoState;

// #########################################################################
// ## Private API ##########################################################
// #########################################################################
void avplay_init();
void avplay_release();
VideoState *avplay_open(const char *filename);
void avplay_close(VideoState *is);
void avplay_set_rendercallback(VideoState *is, render_callback_t cb, void *data);
void avplay_set_endofmediacallback(VideoState *is, endofmedia_callback_t cb, void *data);
void avplay_play(VideoState *is);
void avplay_pause(VideoState *is, bool state);
void avplay_stop(VideoState *is);
void avplay_setloop(VideoState *is, bool state);
bool avplay_isloop(VideoState *is);
void avplay_mute(VideoState *is, bool state);
void avplay_setvolume(VideoState *is, int level);
int avplay_getvolume(VideoState *is);
int64_t avplay_getmediatime(VideoState *is);
int64_t avplay_getcurrentmediatime(VideoState *is);
void avplay_setcurrentmediatime(VideoState *is, int64_t time);
const char * avplay_getmetadata(VideoState *is, const char *id);

