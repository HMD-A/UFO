/**
 * @brief   httpサーバーを起動する
 * @param   動作ディレクトリ
 * @param   ポート番号
 * @retval  0:成功 1:失敗
*/
__declspec(dllimport) int httpServer(const char*,int);

/**
 * @brief   httpサーバーの動作確認
 * @retval  0:停止中 1:動作中
*/
__declspec(dllimport) int httpIsWorking();
