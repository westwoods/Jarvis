# -*- coding: utf-8 -*-
# Created by JHJ on 2017. 2. 17.

import os


def reading(data):
    for x in data:
        os.system('say -r 150 \'' + x + '\'')
