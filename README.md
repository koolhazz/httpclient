httpclient
==========

a simple http client, in C++ 

1, 一共5个类，Url, TcpClient, HttpRequest, HttpResponse, HttpClient. 每个类的功能都很简单清晰。
2，HttpClient处理http事务的主要函数是execute。先查看请求是否合法，然后发送，然后读取并解析响应首行及头部。若有内容实体，则继续读入。
   并处理了chunk数据块，gzip压缩数据，以及重定向。各个步骤若有错误，则execute函数返回错误。
3，目前只是编译通过，应该还有一大堆bug