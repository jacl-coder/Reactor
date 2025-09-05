# Reactor

高性能 C++ Reactor 模式网络服务器示例。

## 特性
- 基于 epoll 的主从 Reactor：主线程负责 accept，I/O 线程各自独立 `EventLoop`。
- 事件抽象：`Epoll / Channel / EventLoop / Acceptor / TcpServer / Connection / Buffer / ThreadPool`。
- 非阻塞 I/O + ET 模式（按需开启写事件）避免空轮询。
- 长度前缀协议示例，支持粘包/半包处理。
- eventfd 任务唤醒，跨线程安全投递回调。
- 定时器（timerfd）+ 连接空闲检测（可扩展心跳/超时策略）。

## 目录层级（版本演进）
```
01..32/         # 按阶段迭代目录（学习/演化过程）
34/ 38/ ...     # 后续增加定时器/超时等版本
```

## 快速构建
```bash
make -C 32     # 在某个版本目录构建
./32/echoserver 127.0.0.1 5005 &
./32/client 127.0.0.1 5005
```

## 示例协议
应用层消息: 4 字节长度(包含负载) + 负载。`Connection::onmessage` 聚合缓冲后按长度切分。

## 后续计划（Roadmap）
- IProtocol 抽象：支持 WebSocket / Protobuf 帧
- Session/鉴权集成（对接上层 Auth 服务）
- Prometheus 指标与结构化日志
- 高水位/背压与限流策略
- EPOLLONESHOT + 零拷贝 (sendfile / writev)

## License
本项目采用 MIT License，详见 `LICENSE`。
