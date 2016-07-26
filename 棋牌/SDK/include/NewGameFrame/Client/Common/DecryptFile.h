#ifndef DECRYPTFILE_HHH
#define DECRYPTFILE_HHH

/// 压缩文件用的FilePos
/// 解密后，会产生一个 PackPosInfo，用stl的map记录了各个文件名所对应的内存地址起点和内存长度
/// 即解密后，只需要 PackPosInfo[文件名] ，即可得到该文件名的文件所对应的内存


/// 将加密压缩包解压多内存
/// fileName压缩包路径  passWord解压密码（明文） pLen 压缩包大小
/// 返回压缩包内存，需要调用free释放
void * DecryptZipFile(const char *fileName,const char*passWord,unsigned long *pLen);

/// 从内存中获取图片
/// fileName 该张图片的相对路径 BufZip压缩包的内存 lenBufZip压缩包的内存长度  pBuf该张图片内存 password解压密码
/// 返回该张图片大小 若为0 则导出失败
unsigned long LoadFileFromZip(const char *fileName,void *BufZip,unsigned int lenBufZip,char **pBuf, const char *pHeadInfo);

#endif