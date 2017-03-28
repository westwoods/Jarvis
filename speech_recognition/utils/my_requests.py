# -*- coding: utf-8 -*-
# Created by JHJ on 2017. 2. 15.

import requests
import re
from bs4 import BeautifulSoup


def escape_tags_and_emails(string):
    # escape tags
    str1 = re.sub(r'<[^<]+?>', '', string)

    # escape emails
    str2 = re.sub(r'[^ ]+@.[^ ]+', '', str1)

    return str2


def get_news():
    try:
        base_url = 'http://news.naver.com'
        response = requests.get(base_url)
        soup = BeautifulSoup(response.text, 'lxml')
        elements = soup.select('div.section_wide > div > ul > li > a.sm2')

        news_url = base_url + elements[0].get('href')
        response = requests.get(news_url)
        soup = BeautifulSoup(response.text, 'lxml')
        elements = soup.select('div#articleBodyContents')
        line_length = len(elements[0].contents)

        data = []

        for i, v in enumerate(elements[0].contents):
            if i in range(9) \
                    or len(escape_tags_and_emails(v.encode('utf-8').strip())) is 0 \
                    or i in range(line_length - 6, line_length):
                continue
            else:
                data.append(escape_tags_and_emails(v.encode('utf-8').strip()))
        return data

    except requests.exceptions.RequestException:
        print('HTTP Request failed')
        return False
