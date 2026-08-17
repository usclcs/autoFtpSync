#include "webserver.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>

namespace {

// 内嵌 Web 管理页面（token 通过 URL 查询参数或本地存储注入）
const char *kPageHtml = R"html(<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>autoFtpSync 远程管理</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body { font-family: "Microsoft YaHei", Arial, sans-serif; background: #0f172a; color: #e2e8f0; padding: 20px; }
  h1 { font-size: 20px; margin-bottom: 4px; }
  .sub { color: #94a3b8; font-size: 13px; margin-bottom: 16px; }
  .card { background: #1e293b; border-radius: 10px; padding: 14px 16px; margin-bottom: 14px; }
  .card h2 { font-size: 15px; color: #38bdf8; margin-bottom: 10px; }
  table { width: 100%; border-collapse: collapse; font-size: 13px; }
  th, td { text-align: left; padding: 7px 8px; border-bottom: 1px solid #334155; }
  th { color: #94a3b8; font-weight: normal; }
  .badge { display: inline-block; padding: 2px 10px; border-radius: 10px; font-size: 12px; }
  .ok   { background: #14532d; color: #4ade80; }
  .warn { background: #713f12; color: #facc15; }
  .err  { background: #7f1d1d; color: #f87171; }
  .idle { background: #1f2937; color: #94a3b8; }
  .srv  { background: #0d9488; }
  button { padding: 4px 12px; margin-right: 6px; border: none; border-radius: 6px;
           background: #0ea5e9; color: #fff; cursor: pointer; font-size: 12px; }
  button:hover { background: #0284c7; }
  button.danger { background: #ef4444; }
  button.danger:hover { background: #dc2626; }
  button.ghost { background: #334155; }
  button.ghost:hover { background: #475569; }
  #log { height: 220px; overflow-y: auto; font-size: 12px; line-height: 1.7; }
  #log div { border-bottom: 1px dashed #334155; padding: 3px 0; }
  .L-INFO { color: #94a3b8; } .L-WARN { color: #facc15; } .L-ERROR { color: #f87171; }
  #tokenBox { margin-bottom: 14px; padding: 10px 14px; border-radius: 10px;
              background: #7f1d1d; color: #fecaca; font-size: 13px; }
  .mt { margin-top: 10px; }
  .stat { display: inline-block; margin-right: 20px; font-size: 13px; }
  .stat b { color: #38bdf8; }
  .modal-mask { position: fixed; left:0; top:0; right:0; bottom:0; background: rgba(0,0,0,.6);
                display:none; z-index: 50; align-items: flex-start; justify-content: center; padding: 40px 16px; }
  .modal { background: #1e293b; border-radius: 10px; padding: 18px 20px; width: 560px;
           max-width: 100%; max-height: 80vh; overflow: auto; }
  .modal h3 { font-size: 15px; margin-bottom: 4px; }
  .modal p.desc { color: #94a3b8; font-size: 12px; margin-bottom: 10px; }
  .modal label { display: block; font-size: 12px; color: #94a3b8; margin: 9px 0 4px; }
  .modal input[type=text], .modal select, .modal textarea { width: 100%; padding: 7px 9px;
      border-radius: 6px; border: 1px solid #334155; background: #0f172a; color: #e2e8f0; font-size: 13px; }
  .modal input[type=checkbox] { vertical-align: middle; }
  .modal .row { display: flex; gap: 12px; } .modal .row > div { flex: 1; }
  .modal .foot { margin-top: 16px; text-align: right; }
  .small { font-size: 12px; color: #94a3b8; }
  a.link { color: #38bdf8; cursor: pointer; text-decoration: none; }
  a.link:hover { text-decoration: underline; }
</style>
</head>
<body>
<h1>autoFtpSync 远程管理 <span class="small" id="stVersion" style="color:#94a3b8">v-</span></h1>
<div class="sub">服务器同步状态监控 · 页面每 3 秒自动刷新</div>
<div id="tokenBox" style="display:none">访问令牌无效或已失效，请在地址栏以 ?token=xxx 方式重新访问。</div>
<div class="card">
  <h2>运行概况</h2>
  <span class="stat">运行中任务：<b id="stRunning">-</b></span>
  <span class="stat">服务器数：<b id="stServers">-</b></span>
  <span class="stat">任务数：<b id="stTasks">-</b></span>
</div>
<div class="card">
  <h2>服务器连接</h2>
  <div id="servers">加载中...</div>
</div>
<div class="card">
  <h2>同步任务 <button style="float:right; margin-right:0" onclick="openTaskEdit(null)">＋ 新增任务</button></h2>
  <table>
    <thead><tr><th>任务名</th><th>服务器</th><th>状态</th><th>已上传</th><th>失败</th><th>操作</th></tr></thead>
    <tbody id="tasks"><tr><td colspan="6">加载中...</td></tr></tbody>
  </table>
</div>
<div class="card">
  <h2>最近日志</h2>
  <div id="log">加载中...</div>
</div>

<!-- 任务编辑弹窗（新增/编辑共用） -->
<div class="modal-mask" id="taskModal">
  <div class="modal">
    <h3 id="taskModalTitle">新增任务</h3>
    <p class="desc">带 * 为必填项。保存后若任务已启用且服务器存在，将自动启动同步。</p>
    <div class="row">
      <div><label>任务名 *</label><input type="text" id="fName"></div>
      <div><label>服务器连接 *</label><select id="fServer"></select></div>
    </div>
    <label>本地目录 *</label><input type="text" id="fLocal" placeholder="如 D:\data 或 /home/user/data">
    <label>远端目录</label><input type="text" id="fRemote" placeholder="留空表示服务器登录根目录">
    <label>忽略规则（每行一条，支持 * 通配）</label>
    <textarea id="fIgnore" rows="3" placeholder="如：*.tmp&#10;__pycache__/"></textarea>
    <label style="margin-top:12px"><input type="checkbox" id="fDeleteRemote"> 本地删除时同步删除服务器文件</label>
    <label><input type="checkbox" id="fEnabled" checked> 启用该任务</label>
    <div class="foot">
      <button class="ghost" onclick="closeTaskEdit()">取消</button>
      <button onclick="saveTask()">保存</button>
    </div>
  </div>
</div>
<script>
var TOKEN = new URLSearchParams(location.search).get('token');
if (TOKEN) localStorage.setItem('wsToken', TOKEN);
TOKEN = TOKEN || localStorage.getItem('wsToken') || '';
function api(url, opts) {
  opts = opts || {};
  var sep = url.indexOf('?') >= 0 ? '&' : '?';
  return fetch(url + sep + 'token=' + encodeURIComponent(TOKEN), opts);
}
function esc(s) {
  return String(s == null ? '' : s).replace(/[&<>"']/g, function (c) {
    return {'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c];
  });
}
function badge(status) {
  var cls = 'idle';
  if (status === '监控中' || status === '同步中' || status === '连接中') cls = 'ok';
  else if (status === '已暂停') cls = 'warn';
  else if (status === '异常' || status.indexOf('失败') >= 0) cls = 'err';
  return '<span class="badge ' + cls + '">' + esc(status) + '</span>';
}
function render(d) {
  if (!d.ok) { return; }
  document.getElementById('stRunning').textContent = d.running;
  document.getElementById('stServers').textContent = d.servers.length;
  document.getElementById('stVersion').textContent = d.version ? ('v' + d.version) : '';
  document.getElementById('stTasks').textContent = d.tasks.length;
  var sHtml = '';
  d.servers.forEach(function (s) {
    sHtml += '<span class="badge srv" style="margin-right:8px">' + esc(s.name) + ' (' + esc(s.host) + ':' + s.port + ') ' + esc(s.protocol) + '</span>';
  });
  document.getElementById('servers').innerHTML = sHtml || '（无服务器连接）';
  var tHtml = '';
  d.tasks.forEach(function (t) {
    var ops = '';
    if (t.status === '停止' || t.status === '已暂停') {
      ops += '<button onclick="ctl(\'start\',\'' + t.id + '\')">启动</button>';
    } else {
      ops += '<button class="danger" onclick="ctl(\'stop\',\'' + t.id + '\')">停止</button>';
    }
    ops += '<button class="ghost" onclick="ctl(\'sync\',\'' + t.id + '\')">立即同步</button>'
      + '<button class="ghost" onclick="ctl(\'retry\',\'' + t.id + '\')">重试失败</button>'
      + '<button class="ghost" onclick="openTaskEdit(\'' + t.id + '\')">编辑</button>'
      + '<button class="ghost" onclick="toggleTask(\'' + t.id + '\')">启用/停用</button>'
      + '<button class="danger" onclick="deleteTask(\'' + t.id + '\')">删除</button>';
    tHtml += '<tr><td>' + esc(t.name) + '</td><td>' + esc(t.server) + '</td><td>' + badge(t.status)
      + '</td><td>' + t.uploaded + '</td><td>' + t.failed + '</td><td>' + ops + '</td></tr>';
  });
  document.getElementById('tasks').innerHTML = tHtml || '<tr><td colspan="6">（无同步任务）</td></tr>';
  var lHtml = '';
  d.logs.slice(-200).reverse().forEach(function (l) {
    lHtml += '<div class="L-' + esc(l.level) + '">[' + esc(l.time) + '] [' + esc(l.task) + '] ' + esc(l.msg) + '</div>';
  });
  document.getElementById('log').innerHTML = lHtml || '<div>（无日志）</div>';
}
function ctl(action, id) {
  api('/api/control', {method: 'POST', headers: {'Content-Type': 'application/json'},
       body: JSON.stringify({action: action, id: id})})
    .then(function (r) { if (r.status === 401) { document.getElementById('tokenBox').style.display = ''; } });
}
// 任务增删改查
var taskCache = []; // tasklist 缓存（供编辑回填）
function loadTaskList(cb) {
  api('/api/tasklist').then(function (r) {
    if (r.status === 401) { document.getElementById('tokenBox').style.display = ''; return; }
    return r.json();
  }).then(function (d) {
    if (d && d.ok) { taskCache = d.tasks || []; if (cb) cb(d); }
  });
}
function openTaskEdit(id) {
  loadTaskList(function (d) {
    // 填充服务器下拉
    var sel = document.getElementById('fServer');
    sel.innerHTML = '';
    (d.servers || []).forEach(function (s) {
      var op = document.createElement('option');
      op.value = s.name; op.textContent = s.name + ' (' + s.host + ':' + s.port + ')';
      sel.appendChild(op);
    });
    document.getElementById('fName').value = '';
    document.getElementById('fLocal').value = '';
    document.getElementById('fRemote').value = '';
    document.getElementById('fIgnore').value = '';
    document.getElementById('fDeleteRemote').checked = false;
    document.getElementById('fEnabled').checked = true;
    document.getElementById('taskModal').dataset.id = '';
    if (id) {
      var t = null;
      taskCache.forEach(function (x) { if (x.id === id) t = x; });
      if (t) {
        document.getElementById('taskModalTitle').textContent = '编辑任务';
        document.getElementById('taskModal').dataset.id = t.id;
        document.getElementById('fName').value = t.name;
        if (t.serverName) sel.value = t.serverName;
        document.getElementById('fLocal').value = t.localDir || '';
        document.getElementById('fRemote').value = t.remoteDir || '';
        document.getElementById('fIgnore').value = (t.ignoreRules || []).join('\n');
        document.getElementById('fDeleteRemote').checked = !!t.deleteRemote;
        document.getElementById('fEnabled').checked = !!t.enabled;
      }
    } else {
      document.getElementById('taskModalTitle').textContent = '新增任务';
    }
    document.getElementById('taskModal').style.display = 'flex';
  });
}
function closeTaskEdit() {
  document.getElementById('taskModal').style.display = 'none';
}
function saveTask() {
  var task = {
    id: document.getElementById('taskModal').dataset.id || '',
    name: document.getElementById('fName').value.trim(),
    serverName: document.getElementById('fServer').value,
    localDir: document.getElementById('fLocal').value.trim(),
    remoteDir: document.getElementById('fRemote').value.trim(),
    enabled: document.getElementById('fEnabled').checked,
    deleteRemote: document.getElementById('fDeleteRemote').checked,
    ignoreRules: document.getElementById('fIgnore').value.split('\n').map(function (s) { return s.trim(); }).filter(function (s) { return s.length > 0; })
  };
  api('/api/task', {method: 'POST', headers: {'Content-Type': 'application/json'},
       body: JSON.stringify({action: 'save', task: task})})
    .then(function (r) { if (r.status === 401) { document.getElementById('tokenBox').style.display = ''; return; }
      return r.json();
    }).then(function (d) {
      if (d && d.ok) { closeTaskEdit(); load(); }
      else if (d && d.error) { alert('保存失败：' + d.error); }
    });
}
function deleteTask(id) {
  if (!confirm('确定删除该任务吗？')) return;
  api('/api/task', {method: 'POST', headers: {'Content-Type': 'application/json'},
       body: JSON.stringify({action: 'delete', task: {id: id}})})
    .then(function (r) { if (r.status === 401) { document.getElementById('tokenBox').style.display = ''; return; }
      return r.json();
    }).then(function (d) { if (d && d.ok) load(); });
}
function toggleTask(id) {
  api('/api/task', {method: 'POST', headers: {'Content-Type': 'application/json'},
       body: JSON.stringify({action: 'toggle', task: {id: id}})})
    .then(function (r) { if (r.status === 401) { document.getElementById('tokenBox').style.display = ''; return; }
      return r.json();
    }).then(function (d) { if (d && d.ok) load(); });
}
function load() {
  api('/api/status').then(function (r) {
    if (r.status === 401) { document.getElementById('tokenBox').style.display = ''; return; }
    return r.json();
  }).then(function (d) { if (d) render(d); });
}
load();
setInterval(load, 3000);
</script>
</body>
</html>
)html";
}

WebServer::WebServer(QObject *parent)
    : QObject(parent)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &WebServer::onNewConnection);
}

bool WebServer::start(quint16 port, const QString &token)
{
    m_token = token;
    if (!m_server->listen(QHostAddress::Any, port)) {
        emit logMessage("ERROR", "Web", QStringLiteral("Web 管理服务监听失败: %1").arg(m_server->errorString()));
        return false;
    }
    emit logMessage("INFO", "Web",
                    QStringLiteral("Web 远程管理已启动: http://本机IP:%1/?token=%2").arg(m_server->serverPort()).arg(m_token));
    return true;
}

void WebServer::stop()
{
    if (m_server && m_server->isListening())
        m_server->close();
}

void WebServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *sock = m_server->nextPendingConnection();
        connect(sock, &QTcpSocket::readyRead, this, [this, sock]() { onReadyRead(sock); });
        connect(sock, &QTcpSocket::disconnected, sock, &QTcpSocket::deleteLater);
    }
}

void WebServer::onReadyRead(QTcpSocket *sock)
{
    const QByteArray req = sock->readAll();
    handleRequest(sock, req);
}

// 解析并处理一个 HTTP 请求（短连接：处理完即断开）
void WebServer::handleRequest(QTcpSocket *sock, const QByteArray &request)
{
    const int headerEnd = request.indexOf("\r\n\r\n");
    const int lineEnd = request.indexOf("\r\n");
    if (headerEnd < 0 || lineEnd < 0) {
        reply(sock, 400, "text/plain", QByteArray("Bad Request"));
        return;
    }
    const QByteArray head = request.left(headerEnd);
    const QByteArray body = request.mid(headerEnd + 4);

    // 请求行：METHOD SP PATH HTTP/x
    const QByteArray line = head.left(lineEnd);
    const QList<QByteArray> parts = line.split(' ');
    if (parts.size() < 3) {
        reply(sock, 400, "text/plain", QByteArray("Bad Request"));
        return;
    }
    const QByteArray method = parts.at(0);
    QByteArray target = parts.at(1);
    // 拆分 path 与 query
    QByteArray path = target;
    QByteArray query;
    const int q = target.indexOf('?');
    if (q >= 0) {
        path = target.left(q);
        query = target.mid(q + 1);
    }

    if (path == "/") {
        reply(sock, 200, "text/html; charset=utf-8", QByteArray(kPageHtml));
        return;
    }

    // 其余 API 均需令牌校验
    if (!checkToken(query)) {
        reply(sock, 401, "application/json; charset=utf-8", QByteArray("{\"ok\":false,\"error\":\"invalid token\"}"));
        return;
    }

    if (path == "/api/status" && method == "GET") {
        QJsonObject obj;
        if (m_statusProvider)
            obj = m_statusProvider();
        obj.insert("ok", true);
        reply(sock, 200, "application/json; charset=utf-8",
              QJsonDocument(obj).toJson(QJsonDocument::Compact));
        return;
    }

    if (path == "/api/control" && method == "POST") {
        const QJsonObject reqObj = QJsonDocument::fromJson(body).object();
        const QString action = reqObj.value("action").toString();
        const QString taskId = reqObj.value("id").toString();
        QString err;
        if (m_controlHandler)
            err = m_controlHandler(action, taskId);
        QJsonObject obj;
        obj.insert("ok", err.isEmpty());
        if (!err.isEmpty())
            obj.insert("error", err);
        reply(sock, 200, "application/json; charset=utf-8",
              QJsonDocument(obj).toJson(QJsonDocument::Compact));
        return;
    }

    // 任务管理 API：GET /api/tasklist 返回任务与服务器列表（查看/编辑表单数据）
    if (path == "/api/tasklist" && method == "GET") {
        QJsonObject obj;
        if (m_taskListProvider)
            obj = m_taskListProvider();
        obj.insert("ok", true);
        reply(sock, 200, "application/json; charset=utf-8",
              QJsonDocument(obj).toJson(QJsonDocument::Compact));
        return;
    }

    // 任务管理 API：POST /api/task  body={action:save|delete|toggle, task:{...}}
    if (path == "/api/task" && method == "POST") {
        const QJsonObject reqObj = QJsonDocument::fromJson(body).object();
        const QString action = reqObj.value("action").toString();
        const QJsonObject task = reqObj.value("task").toObject();
        QString err;
        if (m_taskManagerHandler)
            err = m_taskManagerHandler(action, task);
        QJsonObject obj;
        obj.insert("ok", err.isEmpty());
        if (!err.isEmpty())
            obj.insert("error", err);
        reply(sock, 200, "application/json; charset=utf-8",
              QJsonDocument(obj).toJson(QJsonDocument::Compact));
        return;
    }

    reply(sock, 404, "text/plain", QByteArray("Not Found"));
}

bool WebServer::checkToken(const QByteArray &query) const
{
    if (m_token.isEmpty())
        return false;
    // 支持 query 参数 token 或 X-Token 头（头在 body 前解析较繁琐，此处仅按 query 校验；
    // 页面同时带 X-Token 头以保证一致性，query 为空时头里取不到 -> 由页面带 query 访问）
    QUrlQuery q(QString::fromUtf8(query));
    return q.queryItemValue("token") == m_token;
}

void WebServer::reply(QTcpSocket *sock, int code, const QByteArray &contentType,
                      const QByteArray &body, const QByteArray &extraHeaders)
{
    QByteArray resp = "HTTP/1.1 " + QByteArray::number(code) + " " + (code == 200 ? "OK" : "Error") + "\r\n";
    resp += "Content-Type: " + contentType + "\r\n";
    resp += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
    resp += "Connection: close\r\n";
    if (!extraHeaders.isEmpty())
        resp += extraHeaders + "\r\n";
    resp += "\r\n";
    resp += body;
    sock->write(resp);
    sock->flush();
    sock->disconnectFromHost();
}
