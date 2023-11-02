/// @brief 受信データサイズ
/// @param com COMポート番号
/// @return 受信データバイト数
__declspec(dllimport) int comsize(int com);

/// @brief バッファから1byte読み出し
/// @param com COMポート番号
/// @return 受信データ1byte
__declspec(dllimport) unsigned char comget(int com);

/// @brief バッファから配列読み出し
/// @param com COMポート番号
/// @param p 配列(ポインタ)
/// @param size 要求サイズ
/// @return 受信成功データbyte数
__declspec(dllimport) int comgetA(int com, unsigned char *p, int size);

/// @brief 1byte送信
/// @param com COMポート番号
__declspec(dllimport) void comsend(int com,char send);

/// @brief バッファから配列送信
/// @param com COMポート番号
/// @param p 配列(ポインタ)
/// @param size 送信サイズ
/// @return 送信成功データbyte数
__declspec(dllimport) int comsendA(int com, unsigned char *p, int size);

/// @brief COMポートオープン
/// @param com COMポート番号
/// @param baud ボーレート
/// @return 失敗:-1 成功:0
__declspec(dllimport) int comopen(int com,int baud);

/// @brief COMポートクローズ
/// @param com COMポート番号
__declspec(dllimport) void comclose(int com);
