#ifndef CLIENT_H
#define CLIENT_H

#ifdef __cplusplus
    extern "C" {
#endif

    struct file_tags;
    struct file_info {
        char *file;
        struct file_tags *tags;
        char *title;
        int avg_bitrate;
        int bitrate;
        int rate;
        int curr_time;
        int total_time;
        int channels;
        int state; /* STATE_* */
        char *block_file;
        int block_start;
        int block_end;
    };

    /* Information about the currently played file. */
    int server_connect();
    int ping_server (int sock);
    void close_socket(int sock);
    void server_disconnect(int sock);
    int startPlay(int srv_sock);
    void switchPause (int srv_sock);
    void file_info_reset (struct file_info *f);
    void file_info_block_init (struct file_info *f);
    int get_state (int srv_sock);
    int get_int_from_srv (int srv_sock);
    void wait_for_data (int srv_sock);
    void getCurrentFileInfo(int sock);
    int getFileState();
    void play_it (const char *file);
    void interface_cmdline_playit (int server_sock, char *path);
    void *get_event_data (int serv_sock, const int type);
    void stopPlay(int serv_sock);
    char* getCurFile();
    char *get_str_from_srv (int srv_sock);
    int getTime();
    bool isStop();
    int getCurrentTime(int serv_sock);
    void set_mixer (int serv_sock, int val);
    int get_mixer (int serv_sock);


#ifdef __cplusplus
}
#endif

#endif // CLIENT_H
