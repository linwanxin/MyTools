import requests
from datetime import datetime
import configparser


def process():
    #终于明白他的cookie原理了;
    url = 'http://10.124.142.30:8080/weeklyreport/view/login'
    r = requests.get(url=url)

    for key, value in r.cookies.items():
        cookie = key + '=' + value

    userInfo = getUserInfo()
    ##测试发现必须要+Referer不然会404
    headers = {
        'Cookie': cookie,
        'Referer': 'http://10.124.142.30:8080/weeklyreport/view/calendar.jsp'
    }
    requests.post(url=url, data=userInfo, headers=headers)

    postData = getPost()
    post = requests.post("http://10.124.142.30:8080/weeklyreport/dailyReport/add", data=postData, headers=headers)
    if post.status_code == 200:
        print("写入成功！")
    else:
        print("写入失败！")


def getUserInfo():
    userInfo = {
        'userName': cf.get('userInfo', 'userName'),
        'password': cf.get('userInfo', 'password')
    }
    return userInfo

def getPost():
    nowDate = datetime.now().strftime('%a %b %d %Y %H:%M:%S')
    postData = {
        'updateDate': nowDate,
        'content': cf.get('post', 'content'),
        'createDate': nowDate,
        'typeinfo.id': cf.get('post', 'contentType')
    }
    return postData


if __name__ == '__main__':
    cf = configparser.ConfigParser()
    cf.read('./boss.conf',encoding='utf-8')
    process()