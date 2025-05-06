const grpc = require('@grpc/grpc-js')
const messageProto = require('./proto')
const configModule = require('./config')
const emailModule = require('./email')
const constModule = require('./const')
const svgCaptcha = require('svg-captcha');
const redisModule = require('./redis')

async function GetVerifyCode(call, callback) {
    console.log('email is ', call.request.email)
    try {
        // 查询该邮箱是否已存在
        let queryRes = await redisModule.GetRedis(constModule.code_prefix + call.request.email)
        console.log('queryRes is ', queryRes)
        // 获取之前发送过的验证码
        let uniqueId = queryRes;
        if (queryRes == null) {
            // 不存在则 生成验证码
            const captcha = svgCaptcha.create({
                size: 6,       // 验证码长度
                ignoreChars: 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ', // 忽略字母
                noise: 2,      // 干扰线数量
                color: true    // 彩色验证码
            })
            uniqueId = captcha.text
            console.log('uniqueId is ', uniqueId)
            // 将验证码保存到redis中 三分钟过期
            let bres = await redisModule.SetRedisExpire(constModule.code_prefix + call.request.email, uniqueId, 180)
            if (!bres) {
                // redis 操作失败 编辑回包
                callback(null, {
                    error: constModule.Errors.RedisErr,
                    email: call.request.email,
                });
                return
            }
        }
        // 编辑发送的文本
        let emailText = '你已选择此邮箱地址作为你的 ChatRoom 账户。为验证此电子邮件地址属于你，' +
            '请在电子邮件验证页输入下方验证码：\n\n' + uniqueId + '\n\n此邮件发出三分钟后，验证码过期。\n\n' +
            '你收到此电子邮件的原因：\n' +
            'ChatRoom 会在你选择电子邮件地址为 ChatRoom 账户时提出验证要求。你的 ChatRoom 账户需经过验证才能使用。'
        // 编辑发送Options
        let mailOptions = {
            from: configModule.email_user,
            to: call.request.email,
            subject: '验证码',
            text: emailText,
        };

        // 阻塞来发送信息
        let sendRes = await emailModule.SendMail(mailOptions);
        console.log("send res is ", sendRes)
        if (!sendRes) {
            // SendMail 操作失败 编辑回包
            callback(null, {
                error: constModule.Errors.SendMailErr,
                email: call.request.email,
            });
            return
        }

        // 编辑回包
        callback(null, {
            error: constModule.Errors.Success,
            email: call.request.email,
            code: uniqueId
        });
    } catch (error) {
        // 发送失败
        console.log("catch error is ", error)
        // 编辑回包
        callback(null, {
            email: call.request.email,
            error: constModule.Errors.Exception
        });
    }
}

function main() {
    // 创建GRPC服务器
    let server = new grpc.Server()
    // 为服务器添加服务
    server.addService(messageProto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    // 开启服务
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), (err, port) => {
        console.log(`grpc server started, port is ${port}`)
    })
}

main()