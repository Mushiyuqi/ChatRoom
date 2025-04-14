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
  return new Promise(function (resolve, reject) {
    transport.sendMail(mailOptions, function (error, info) {
      if (error) {
        console.log(error)
        reject(error)
      } else {
        console.log('邮件已发送成功: ' + info.response)
        resolve(info.response)
      }
    })
  })
}

module.exports.SendMail = SendMail