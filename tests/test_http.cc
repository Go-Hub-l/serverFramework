#include "http/http.h"
#include "log.h"

void test_request() {
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setHeader("host", "www.xlb.design");
    req->setBody("hello bingbing");
    req->dump(std::cout) << std::endl;
}

void test_response() {
    sylar::http::HttpResponse::ptr rsp(new sylar::http::HttpResponse);
    rsp->setHeader("X-X", "jingjing");
    rsp->setBody("hello jingjing");
    rsp->setStatus((sylar::http::HttpStatus) 400);
    rsp->setClose(false);

    rsp->dump(std::cout) << std::endl;
}

int main(int argc, char** argv) {
    test_request();
    test_response();
    return 0;
}
