import numpy as np
from loadData import DataLoader, Site, User
import math
import os
from util import softmax

PATH = '../../data/'
BETA = 0.

data = DataLoader(PATH)
suMatrix = data.qos_matrix  # s*u
usMatrix = data.qos_matrix.T  # u*s
suVector = np.sum(suMatrix, axis=-1)
suVector = suVector.T
occupiedByTime = np.zeros((data.n_clock, data.n_site))
file = open('../../output/solution.txt', 'w')
for i in range(0, data.n_clock):
    siteRetain = data.site_width.copy()
    clock_allocate = np.zeros_like(data.site_width)
    for j in range(0, data.n_user):
        demand = data.demand[i][j]
        connect_site = usMatrix[j]
        siteNum = np.multiply(suVector, connect_site)

        curSiteIdx = np.where(siteNum > 0)
        curSite = siteNum[curSiteIdx]
        siteScore = 1. / curSite
        curRetainSite = siteRetain[curSiteIdx]
        retainScore = curRetainSite / np.max(curRetainSite)
        weight = np.multiply(siteScore*0, retainScore)
        siteWeight = softmax(weight)
        allWeight = np.zeros_like(siteRetain)
        allWeight[curSiteIdx] = siteWeight
        allocateVector = np.floor(allWeight * demand)
        # 判断是否超出bandwidth
        outBand = allocateVector - siteRetain
        outBand[np.where(outBand < 0)] = 0
        allocateVector -= outBand
        siteRetain -= allocateVector
        demand -= np.sum(allocateVector, axis=-1)

        retain = demand
        # 找出有连接site中retain最大的那个
        availableSite = siteRetain[curSiteIdx]
        maxIdx = curSiteIdx[0][np.argmax(availableSite)]
        allocateVector[maxIdx] += retain
        siteRetain[maxIdx] -= retain

        allocateIdx = np.where(allocateVector > 0)
        allocateWidth = allocateVector[allocateIdx]

        clock_allocate += allocateVector
        uname = data.id2user[j]
        sname = data.id2site[allocateIdx]
        file.write(uname + ':')
        for k in range(0, len(allocateWidth)):
            file.write('<' + sname[k] + ',' + str(int(allocateWidth[k])) + '>')
            if k != len(allocateWidth) - 1:
                file.write(',')

        if (i != data.n_clock - 1) or (j != data.n_user - 1):
            file.write('\n')

    occupiedByTime[i] = clock_allocate

occupiedByTime = occupiedByTime.T
point = math.ceil(data.n_clock*0.95)
occupiedByOrder = np.sort(occupiedByTime, axis=1)
pointBandWidth = occupiedByOrder[:, point-1]
total = np.sum(pointBandWidth)
print(total)