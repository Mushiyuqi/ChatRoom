const nodemailer = require('nodemailer')
const config_module = require('./config')

/**
 * 创建发送邮件的代理
 */
let transport = nodemailer.createTransport({
  host: 'smtp.163.com',
  port: 465,
  secure: true,
  auth: {
    user: config_module.email_user, // 发送方邮件地址
    pass: config_module.email_pass  // 邮箱授权码
  }
})

/**
 * 发送邮件
 * @param {*} mailOptions 发送邮件的参数
 */
function SendMail(mailOptions) {
  // 将异步回调包装成Promise
  return new Promise(function (resolve, reject) {
    transport.sendMail(mailOptions, function (error, info) {
      if (error) {
        // 异常返回
        console.log(error)
        reject(error)
      } else {
        // 正常返回
        console.log('邮件已发送成功: ' + info.response)
        resolve(info.response)
      }
    })
  })
}

module.exports.SendMail = SendMail