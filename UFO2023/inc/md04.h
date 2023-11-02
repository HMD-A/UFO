/**
  * @brief  MDをCCCVで駆動します
  * @param  com: 使用するCOMポート
  * @param  id : 駆動するMD(0～15)
  * @param  crt: MDの出力電流 ×10mA (0~245)
  * @param  pow: MDの出力電圧 ‰ (±1000)
  *
  * @retval 無し
  */
__declspec(dllimport) void MD04CCCV(int com,unsigned char id,int crt,int pow);
