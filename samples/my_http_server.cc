#include "http/http_server.h"
#include "log.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
sylar::IOManager::ptr worker;
void run() {
    g_logger->setLevel(sylar::LogLevel::INFO);
    sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8020");
    if (!addr) {
        SYLAR_LOG_ERROR(g_logger) << "get address error";
        return;
    }

    //sylar::http::HttpServer::ptr http_server(new sylar::http::HttpServer(true, worker.get()));
    sylar::http::HttpServer::ptr http_server(new sylar::http::HttpServer(true));
    bool ssl = false;
    while (!http_server->bind(addr, ssl)) {
        SYLAR_LOG_ERROR(g_logger) << "bind " << *addr << " fail";
        sleep(1);
    }

    if (ssl) {
        //http_server->loadCertificates("/home/apps/soft/sylar/keys/server.crt", "/home/apps/soft/sylar/keys/server.key");
    }

    http_server->start();
}

int main(int argc, char** argv) {
    /*一个线程时无响应，因为一个线程一直在accept，
    不会主动让出执行权，故需两个线程，
    一个bug,sylar的里面可以一个线程，
    我想应该是添加了定时器，之后调试*/
    sylar::IOManager iom(2);
    //worker.reset(new sylar::IOManager(4, false));
    iom.schedule(run);
    return 0;
}
