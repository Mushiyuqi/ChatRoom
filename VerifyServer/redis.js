const configModule = require('./config')
const Redis = require('ioredis')

// 创建redis客户端实例
const RedisClient = new Redis({
    // redis 服务器主机
    host: configModule.redis_host,
    // redis 服务器端口
    port: configModule.redis_port,
    // redis 服务器密码
    password: configModule.redis_password,
})

/**
 * redis 错误监听
 */
RedisClient.on('error', (err) => {
    console.log('redis error: ', err)
    RedisClient.quit()
})

/**
 * redis 根据Key 获取 Value
 */
async function GetRedis(key) {
    try {
        const result = await RedisClient.get(key)
        if (result === null) {
            console.log('redis get error: ', error)
            return null
        }
        console.log('redis get: ' + key + ' success: ', result)
        return result
    } catch (error) {
        console.log('redis get error: ', error)
        return null
    }
}

/**
 * 查询key是否存在
 */
async function QueryRedis(key) {
    try {
        const result = await RedisClient.exists(key)
        if (result === null) {
            console.log('redis exists error: ', error)
            return null
        }
        console.log('redis exists: ' + key + ' success!')
        return result
    } catch (error) {
        console.log('redis get error: ', error)
        return null
    }
}

/**
 * 设置Key和Value并设置过期时间
 */
async function SetRedisExpire(key, value, exptime) {
    try {
        // 设置key和value
        await RedisClient.set(key, value)
        // 设置过期时间
        await RedisClient.expire(key, exptime)
        return true
    } catch (error) {
        console.log('redis set or expire error: ', error)
        return false
    }
}

/**
 * 退出函数
 */
function Quit() {
    RedisClient.quit()
}

/**
 * 导出
 */
module.exports = {
    GetRedis,
    QueryRedis,
    SetRedisExpire,
    Quit
}