#include "esp_log.h"
#include "esp_err.h"
#include "hal_i2c.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_i2s.h"
#include "aplay.h"
#include "mad.h"

#define TAG "aplay"
#define READBUFSZ 2889

struct file_bufer {
  unsigned char *buf;
  unsigned long length;
  uint32_t flen;
  uint32_t fpos;
  FILE* f;
};

void aplay_wav(char* filename){
	//"/sdcard/test.wav"
	WAV_HEADER wav_head;
	FILE *f= fopen(filename, "r");
    if (f == NULL) {
        ESP_LOGE(TAG,"Failed to open file:%s",filename);
        return;
    }
    //fprintf(f, "Hello %s!\n", card->cid.name);
    int rlen=fread(&wav_head,1,sizeof(wav_head),f);
    if(rlen!=sizeof(wav_head)){
        printf("%s\n","read faliled");
        return;
    }
    int channels = wav_head.wChannels;
    int frequency = wav_head.nSamplesPersec;
    int bit = wav_head.wBitsPerSample;
    int datalen= wav_head.wSampleLength;
    printf("channels:%d,frequency:%d,bit:%d\n",channels,frequency,bit);
    char* samples_data = malloc(1024);
   	do{
   		rlen=fread(samples_data,1,1024,f);
   		//datalen-=rlen;
    	hal_i2s_write(0,samples_data,rlen,5000);
    }while(rlen>0);
    fclose(f);
    free(samples_data);
    f=NULL;
}
// static enum mad_flow input_func(void *data, struct mad_stream *stream);
// static enum mad_flow output_func(void *data,struct mad_header const *header,struct mad_pcm *pcm);
// static enum mad_flow error_func(void *data,struct mad_stream *stream,struct mad_frame *frame);
// //static enum mad_flow (*header_func)(void *, struct mad_header const *);
// //static enum mad_flow (*filter_func)(void *, struct mad_stream const *, struct mad_frame *);
// //static enum mad_flow (*message_func)(void *, void *, unsigned int *);

// void aplay_mp3(char* filename){
// 	if(filename==NULL)
// 		return;
// 	struct mad_decoder* decoder=NULL;
//     decoder=malloc(sizeof(struct mad_decoder));
//     if(decoder==NULL){
//         ESP_LOGI(TAG,"malloc mad decoder failed");
//         goto fail;
//     }
//     struct file_bufer mp3_file;
//     mp3_file.buf=malloc(READBUFSZ);
//     if(mp3_file.buf==NULL){
//         ESP_LOGE(TAG,"malloc buf failed");
//         goto fail;
//     }
//     mp3_file.length=READBUFSZ;
//     mp3_file.f= fopen(filename, "r");
//     if (mp3_file.f == NULL) {
//         ESP_LOGE(TAG,"Failed to open file:%s",filename);
//         goto fail;
//     }
//     //get file length
//     fseek (mp3_file.f, 0, SEEK_END);
//     mp3_file.flen=ftell(mp3_file.f);
//     fseek (mp3_file.f, 0, SEEK_SET);
//     ESP_LOGI(TAG,"open mp3 file:%s,length:%d",filename,mp3_file.flen);
//     int result;
//     mad_decoder_init(decoder,&mp3_file,input_func,NULL,NULL,output_func,error_func,NULL);
//     result = mad_decoder_run(decoder, MAD_DECODER_MODE_SYNC);
//     ESP_LOGI(TAG,"decoder result :%d",result);
//     mad_decoder_finish(decoder);
// fail:
//     //ESP_LOGE(TAG,"malloc failed!");
//     if(decoder!=NULL)
//         free(decoder);
//     if(mp3_file.buf!=NULL)
//         free(mp3_file.buf);
//     if(mp3_file.f!=NULL){
//         fclose(mp3_file.f);
//     }
// }
// /*
// fixed point 24 to int
// */
// static inline signed int scale(mad_fixed_t sample)
// {
//       /* round */
//       sample += (1L << (MAD_F_FRACBITS - 16));

//       /* clip */
//       if (sample >= MAD_F_ONE)
//         sample = MAD_F_ONE - 1;
//       else if (sample < -MAD_F_ONE)
//         sample = -MAD_F_ONE;

//       /* quantize */
//       return sample >> (MAD_F_FRACBITS + 1 - 16);
// }

// static enum mad_flow input_func(void *data, struct mad_stream *stream){
//     struct file_bufer *mp3fp;
//     int ret_code;
//     int unproc_data_size; /*the unprocessed data's size*/
//     int copy_size;
//     int size;
//     //ESP_LOGI(TAG,"MAD INPUT");
//     mp3fp = (struct file_bufer *)data;
//     if(mp3fp->fpos < mp3fp->flen) {
//         unproc_data_size = stream->bufend - stream->next_frame;
//         //printf("%d, %d, %d\n", unproc_data_size, mp3fp->fpos, mp3fp->fbsize);
//         memcpy(mp3fp->buf, mp3fp->buf + mp3fp->flen - unproc_data_size, unproc_data_size);
//         copy_size = mp3fp->length - unproc_data_size;
//         if(mp3fp->fpos + copy_size > mp3fp->flen) {
//             copy_size = mp3fp->flen - mp3fp->fpos;
//         }
//         fread(mp3fp->buf+unproc_data_size, 1, copy_size, mp3fp->f);
//         size = unproc_data_size + copy_size;
//         mp3fp->fpos += copy_size;

//         /*Hand off the buffer to the mp3 input stream*/
//         mad_stream_buffer(stream, mp3fp->buf, size);
//         ret_code = MAD_FLOW_CONTINUE;
//     } else {
//         ret_code = MAD_FLOW_STOP;
//     }

//     return ret_code;
// }
// static enum mad_flow error_func(void *data,
// 		    struct mad_stream *stream,
// 		    struct mad_frame *frame)
// {
//     struct file_bufer *mp3_file = data;
//     ESP_LOGE(TAG, "decoding error 0x%04x (%s) at byte offset %u\n",
//         stream->error, mad_stream_errorstr(stream),
//         stream->this_frame - mp3_file->buf);
    
//     /* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
    
//     return MAD_FLOW_CONTINUE;
// }
// static enum mad_flow output_func(void *data,
// 		     struct mad_header const *header,
// 		     struct mad_pcm *pcm)
// {
//   unsigned int nchannels, nsamples;
//   mad_fixed_t const *left_ch, *right_ch;

//   /* pcm->samplerate contains the sampling frequency */
//   static int init=0;
//   if(init==0){
//     ESP_LOGI(TAG,"sample rate:%d,channel num:%d",pcm->samplerate,pcm->channels);
//     init=1;
//   }
//   nchannels = pcm->channels;
//   nsamples  = pcm->length;
//   left_ch   = pcm->samples[0];
//   right_ch  = pcm->samples[1];
//   int len=0;     
//   char buf[4];
//   while (nsamples--) {
//     signed int sample;
//     /* output sample(s) in 16-bit signed little-endian PCM */

//     sample = scale(*left_ch++);
//         buf[0]=((sample>>0)&0xff);
//         buf[1]=((sample>>8)&0xff);
//         len+=2;
// //    putchar((sample >> 0) & 0xff);
// //    putchar((sample >> 8) & 0xff);

//     if (nchannels == 2) {
//         sample = scale(*right_ch++);
//         buf[2]=((sample>>0)&0xff);
//         buf[3]=((sample>>8)&0xff);
//         len+=2;
// //      putchar((sample >> 0) & 0xff);
// //      putchar((sample >> 8) & 0xff);
//     }
//     i2s_write_bytes(0,buf, 4, 1000 / portTICK_RATE_MS);
//   }

//   return MAD_FLOW_CONTINUE;
// }




