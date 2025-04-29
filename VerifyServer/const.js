const { SendMail } = require("./email");

let code_prefix = "code_";

const Errors = {
  Success: 0,
  RedisErr: 1,
  Exception: 2,
  SendMailErr: 3,
};


module.exports = { code_prefix, Errors }
