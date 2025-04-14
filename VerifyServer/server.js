const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const config_module = require('./config')
const email_module = require('./email')
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid')

async function GetVerifyCode(call, callback) {
    console.log('email is ', call.request.email)
    try {
        uniqueId = uuidv4()
        console.log('uniqueId is ', uniqueId)
        // 编辑发送Options
        let mailOptions = {
            from: config_module.email_user,
            to: call.request.email,
            subject: '验证码',
            text: '您的验证码为' + uniqueId + '请三分钟内完成注册',
        };
        // 阻塞来发送信息
        let send_res = await email_module.SendMail(mailOptions);
        console.log("send res is ", send_res)
        // 编辑回包
        callback(null, {
            error: const_module.Errors.Success,
            email: call.request.email,
            code: uniqueId
        });
    } catch (error) {
        // 发送失败
        console.log("catch error is ", error)
        // 编辑回包
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}

function main() {
    // 创建GRPC服务器
    var server = new grpc.Server()
    // 为服务器添加服务
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    // 开启服务
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), (err, port) => {
        console.log(`grpc server started, port is ${port}`)
    })
}

main()