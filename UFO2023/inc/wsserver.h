#define CONNECTIONS 256
#define BUFSIZE 2048

typedef struct ResData{
    int len;
    char* data;
    int id;
    char* ip;
} RESDATA;


/*バッファのデータを読みだす*/
RESDATA *wsget();
/*サーバーモード時送信する*/
void wssend(char *msg,int lenght,int id);
/*サーバーモード初期化*/
void wsinit(int port);
/*指定IDの使用を取得*/
char wsisopen(int id);
/*接続数を取得*/
int wsgetconnections();
/*サーバー終了*/
void wsclose();
