const path = require('path')
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

const PROTO_PATH = path.join(__dirname, 'message.proto')
const packageDefinition = protoLoader.loadSync(PROTO_PATH,
  { keepCase: true, longs: String, enums: String, defaults: true, oneofs: true })

// grpc解析proto
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)

const messageProto = protoDescriptor.message

// 导出
module.exports = messageProto

