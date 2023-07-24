// CodeCraft-2022.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <string>
#include <sys/stat.h>
#include <regex>
#include <cstdio>
#include <math.h>
#include <map>

using namespace std;


void getQosConstraint(string configFilename, int& qosConstraint, int& baseCost)
{
	ifstream qosFile;
	qosFile.open(configFilename, ios::in);
	string qosConstraintStr = "";
	//qosConstraint
	qosFile >> qosConstraintStr;
	qosFile >> qosConstraintStr;
	qosConstraint = atoi(qosConstraintStr.substr(qosConstraintStr.find("=") + 1, qosConstraintStr.size()).c_str());
	qosFile >> qosConstraintStr;
	baseCost = atoi(qosConstraintStr.substr(qosConstraintStr.find("=") + 1, qosConstraintStr.size()).c_str());
	qosFile.close();
}

void readCsvQosMap(string Filename, map<string, map<string, int>>& qosMap, vector<string>& customerIdVec) {
	ifstream csvFile;
	map<string, int> y;
	csvFile.open(Filename, ios::in);
	string csvHead = "", csvLineStr = "";
	csvFile >> csvHead;
	csvHead += ',';
	size_t csvHeadpos = csvHead.find(',');
	csvHead = csvHead.substr(csvHeadpos + 1, csvHead.size());
	csvHeadpos = csvHead.find(',');
	// 存储customerIDVec
	while (csvHeadpos != csvHead.npos)
	{
		customerIdVec.emplace_back(csvHead.substr(0, csvHeadpos));
		csvHead = csvHead.substr(csvHeadpos + 1, csvHead.size());
		csvHeadpos = csvHead.find(',');
	}
	// 存储每time的数据
	int customerIDSize = customerIdVec.size();
	while (csvFile >> csvLineStr)
	{
		map<string, int> csvLine;
		csvLineStr += ',';
		size_t pos = csvLineStr.find(',');
		string TimeLineNameTemp = csvLineStr.substr(0, pos);
		y[csvLineStr.substr(0, pos)] = 0;
		csvLineStr = csvLineStr.substr(pos + 1, csvLineStr.size());
		pos = csvLineStr.find(',');
		for (int i = 0; i < customerIDSize; i++) {
			csvLine[customerIdVec[i]] = atoi(csvLineStr.substr(0, pos).c_str());
			csvLineStr = csvLineStr.substr(pos + 1, csvLineStr.size());
			pos = csvLineStr.find(',');
		}
		qosMap[TimeLineNameTemp] = csvLine;
	}
	csvFile.close();
}
void readCsvBandWidthMap(string Filename, map<string, int>& bandWithMap) {
	ifstream csvFile;
	csvFile.open(Filename, ios::in);
	string csvHead = "", csvLineStr = "";
	csvFile >> csvHead;
	while (csvFile >> csvLineStr)
	{
		//csvLineStr += ',';
		size_t pos = csvLineStr.find(',');
		string csvStr = csvLineStr.substr(0, pos);
		string csvValue = csvLineStr.substr(pos + 1, csvLineStr.size());
		bandWithMap[csvStr] = atoi(csvValue.c_str());
	}
	csvFile.close();
}
void readCsvDemandMap(string Filename, map<string, map<string, map<string, int>>>& demandMap, map<string, map<string, int>>& demandCustomerSumMap) {
	vector<string> timeVec;
	vector<string> customerID;
	ifstream csvFile;
	csvFile.open(Filename, ios::in);
	string csvHead = "", csvLineStr = "";
	csvFile >> csvHead;
	csvHead += ',';
	// 去除mtime和stream_id
	size_t csvHeadpos = csvHead.find(',');
	csvHead = csvHead.substr(csvHeadpos + 1, csvHead.size());
	csvHeadpos = csvHead.find(',');
	csvHead = csvHead.substr(csvHeadpos + 1, csvHead.size());
	csvHeadpos = csvHead.find(',');
	// 存储customerID
	while (csvHeadpos != csvHead.npos)
	{
		customerID.emplace_back(csvHead.substr(0, csvHeadpos));
		csvHead = csvHead.substr(csvHeadpos + 1, csvHead.size());
		csvHeadpos = csvHead.find(',');
	}
	// 存储每time的数据
	int customerIDSize = customerID.size();
	string TimeLineNameTemp = "";
	vector<vector<int>> curStreamCustomer;
	vector<string> streamID;
	while (csvFile >> csvLineStr)
	{
		csvLineStr += ',';
		size_t pos = csvLineStr.find(',');
		if (csvLineStr.substr(0, pos) != TimeLineNameTemp) {
			if (TimeLineNameTemp != "") {
				map<string, map<string, int>> curDemand;
				for (int i = 0; i < customerIDSize; i++) {
					int total = 0;
					for (int j = 0; j < streamID.size(); j++) {
						curDemand[customerID[i]][streamID[j]] = curStreamCustomer[j][i];
						total += curStreamCustomer[j][i];
					}
					demandCustomerSumMap[TimeLineNameTemp][customerID[i]] = total;
				}
				demandMap[TimeLineNameTemp] = curDemand;
				vector<vector<int>>().swap(curStreamCustomer);
				vector<string>().swap(streamID);
			}
			TimeLineNameTemp = csvLineStr.substr(0, pos);
			timeVec.emplace_back(TimeLineNameTemp);
		}
		csvLineStr = csvLineStr.substr(pos + 1, csvLineStr.size());
		pos = csvLineStr.find(',');
		string streamIdTemp = csvLineStr.substr(0, pos);
		streamID.emplace_back(streamIdTemp);
		csvLineStr = csvLineStr.substr(pos + 1, csvLineStr.size());
		pos = csvLineStr.find(',');
		int demandStreamTempSum = 0;
		vector<int> streamDemand;
		for (int i = 0; i < customerIDSize; i++) {
			streamDemand.emplace_back(atoi(csvLineStr.substr(0, pos).c_str()));
			csvLineStr = csvLineStr.substr(pos + 1, csvLineStr.size());
			pos = csvLineStr.find(',');
		}
		curStreamCustomer.emplace_back(streamDemand);
	}
	map<string, map<string, int>> curDemand;
	for (int i = 0; i < customerIDSize; i++) {
		int total = 0;
		for (int j = 0; j < streamID.size(); j++) {
			curDemand[customerID[i]][streamID[j]] = curStreamCustomer[j][i];
			total += curStreamCustomer[j][i];
		}
		demandCustomerSumMap[TimeLineNameTemp][customerID[i]] = total;
	}
	demandMap[TimeLineNameTemp] = curDemand;
	vector<vector<int>>().swap(curStreamCustomer);
	vector<string>().swap(streamID);
	csvFile.close();
}
void getConformQosByCustomer(int qosConstraint, vector<vector<int>>& qosVec, vector<vector<int>>& conformQocVec)
{
	int row = qosVec.size(), col = qosVec[0].size();
	for (int i = 0; i < col; i++)
	{
		vector<int>conformIndex;
		for (int j = 0; j < row; j++)
		{
			if (qosVec[j][i] < qosConstraint)
			{
				qosVec[j][i] = 1;
				conformIndex.emplace_back(j);
			}
			else { qosVec[j][i] = 0; }
		}
		conformQocVec.emplace_back(conformIndex);
	}
}


double getScore(vector<vector<int>>& bandUseVec, vector<int>& bandWidthVec, int tile95, int baseCost)
{
	double score = 0;
	for (size_t i = 0; i < bandUseVec.size(); i++)
	{
		vector<int> tmp(bandUseVec[i].begin(), bandUseVec[i].end());
		sort(tmp.begin(), tmp.end());
		if (tmp[tmp.size() - 1] == 0)
			continue;
		else if (tmp[tile95] <= baseCost)
			score += baseCost;
		else
		{
			score += 1.0 / bandWidthVec[i] * pow((tmp[tile95] - baseCost) * 1.0, 2) + tmp[tile95];
			//cout << tmp[tile95] << endl;
		}
	}
	return score;
}

int main()
{
	cout << "HGHGB" << endl;
	clock_t start_time, end_time;
	start_time = clock();
	string bathpath = "/";
	string configFilename = bathpath + "data/config.ini";
	string demandFilename = bathpath + "data/demand.csv";
	string qosFilename = bathpath + "data/qos.csv";
	string bandWidthFilename = bathpath + "data/site_bandwidth.csv";
	string outputFilename = bathpath + "output/solution.txt";
	// 时间mtime 客户节点customerId 流streamId
	map<string, map<string, map<string, int>>> demandMap;
	// siteName 客户节点customerId
	map<string, map<string, int>> qosMap;
	// siteName
	map<string, int> bandWidthMap;
	// 时间mTime 客户customerId 该时间客户的需求总值
	map<string, map<string, int>> demandCustomerSumMap;

	// 客户节点名列表
	vector<string> customerID;
	//各时间流列表
	vector<vector<string>>timeStreamID;
	//各时间流个数
	vector<int> timeStreamNum;
	// 时间列表
	vector<string> timeVec;
	// 边缘节点名列表
	vector<string> siteNameVec;
	// 时间 客户 流
	vector<vector<vector<int>>> demandVec;
	//时间 客户
	vector<vector<int>> demandByCustomerVec;
	// qos
	vector<vector<int>> qosVec;
	//边缘节点带宽
	vector<int> bandWidthVec;
	// qos二值化
	vector<vector<int>>conformQocVecCustomer;
	// qos限定值 basecost
	int qosConstraint = -1, baseCost = -1;

	//获取数据
	cout << "begin read files" << endl;
	getQosConstraint(configFilename, qosConstraint, baseCost);

	readCsvQosMap(qosFilename, qosMap, customerID);
	for (auto iter = qosMap.begin(); iter != qosMap.end(); iter++) {
		vector<int> qosLineTemp;

		for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{
			qosLineTemp.emplace_back(iter2->second);
		}
		qosVec.emplace_back(qosLineTemp);
	}
	readCsvBandWidthMap(bandWidthFilename, bandWidthMap);
	readCsvDemandMap(demandFilename, demandMap, demandCustomerSumMap);
	// qosMap转vec

	// bandWidthMap转vec，获取边缘节点名列表SiteNameVec
	for (auto iter = bandWidthMap.begin(); iter != bandWidthMap.end(); iter++) {
		bandWidthVec.emplace_back(iter->second);
		siteNameVec.emplace_back(iter->first);
	}
	// demand
	for (auto iter = demandMap.begin(); iter != demandMap.end(); iter++) {
		vector<vector<int>> demandLineTemp;
		int first = 0;
		for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{
			vector<int> streamLineTemp;
			vector<string> streamID;
			for (auto iter3 = iter2->second.begin(); iter3 != iter2->second.end(); iter3++)
			{
				if (first == 0)
					streamID.emplace_back(iter3->first);
				streamLineTemp.emplace_back(iter3->second);
			}
			if (first == 0) {
				timeStreamID.emplace_back(streamID);
				timeStreamNum.emplace_back(streamID.size());
				first = 1;
			}
			demandLineTemp.emplace_back(streamLineTemp);
		}
		timeVec.emplace_back(iter->first);
		demandVec.emplace_back(demandLineTemp);
	}
	for (auto iter = demandCustomerSumMap.begin(); iter != demandCustomerSumMap.end(); iter++) {
		vector<int> curDemand;
		for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
		{
			curDemand.emplace_back(iter2->second);
		}
		demandByCustomerVec.emplace_back(curDemand);
	}

	getConformQosByCustomer(qosConstraint, qosVec, conformQocVecCustomer);
	//时间长度，客户数，节点数
	int timeLen = timeVec.size(), customerNum = customerID.size(), siteNum = siteNameVec.size();

	//每个时刻每个客户各流分配情况
	int maxStreamNum = 0;
	for (int i = 0; i < timeLen; i++) {
		maxStreamNum = max(timeStreamNum[i], maxStreamNum);
	}
	vector<vector<vector<vector<bool>>>> mTimeBandCustomerSiteStream(timeLen, vector<vector<vector<bool>>>(
		customerNum, vector<vector<bool>>(siteNum, vector<bool>(maxStreamNum, 0))));
	vector<vector<vector<vector<bool>>>> mMaxTimeBandCustomerSiteStream(timeLen, vector<vector<vector<bool>>>(
		customerNum, vector<vector<bool>>(siteNum, vector<bool>(maxStreamNum, 0))));
	//vector<vector<vector<vector<int>>>> mTimeBandCustomerSiteStream(8928, vector<vector<vector<int>>>(
	//	35, vector<vector<int>>(135, vector<int>(100, 0))));
	//for (int i = 0; i < timeLen; i++){
	//	vector<vector<vector<int>>> timeTmp;
	//	for (int k = 0; k < customerNum; k++) {
	//		vector<vector<int>>tmp(siteNum, vector<int>());
	//		timeTmp.emplace_back(tmp);
	//	}
	//	mTimeBandCustomerSiteStream.emplace_back(timeTmp);
	//}
	//每个时刻每个节点分配情况
	vector<vector<int>> mTimeBand(siteNum, vector<int>(timeLen, 0));
	//每个时刻每个节点能分配到的最值
	vector<vector<int>> mSiteMaxBand(siteNum, vector<int>(timeLen, 0));

	//95%分位
	int tile95 = ceil(timeVec.size() * 0.95) - 1;
	//摆烂点个数
	int maxAFifthNum = timeVec.size() - (tile95 + 1);
	vector<vector<int>> maxAFifthIndex(siteNum, vector<int>(timeLen, 0));
	vector<int>conformQosNumBySiteVec(siteNum, 0);
	for (int i = 0; i < siteNum; i++) {
		int sum = 0;
		for (int j = 0; j < customerNum; j++) {
			if (qosVec[i][j] == 1) {
				sum += 1;
			}
		}
		conformQosNumBySiteVec[i] = sum;
	}
	int conformQocSiteNum = 0;
	for (int i = 0; i < siteNum; i++) {
		if (conformQosNumBySiteVec[i] > 0)
			conformQocSiteNum += 1;
	}
	int totalMaxNum = 0;
	vector<int>siteMaxTimeNum(siteNum, 0);

	for (int i = 0; i < timeLen; i++) {
		for (int j = 0; j < siteNum; j++) {
			int siteDemandSum = 0;
			for (int k = 0; k < customerNum; k++) {
				if (qosVec[j][k] == 1) {
					siteDemandSum += demandByCustomerVec[i][k];
				}
			}
			if (siteDemandSum < bandWidthVec[j]) {
				for (int k = 0; k < customerNum; k++) {
					if (qosVec[j][k] == 1) {
						for (int n = 0; n < timeStreamNum[i]; n++) {
							if (demandVec[i][k][n] > 0) {
								mMaxTimeBandCustomerSiteStream[i][k][j][n] = 1;
								mSiteMaxBand[j][i] += demandVec[i][k][n];
							}
						}
					}
				}
			}
			else if (siteDemandSum >= bandWidthVec[j])
			{
				int total = min(bandWidthVec[j], siteDemandSum);
				vector<vector<int>> tmpVec(demandVec[i].begin(), demandVec[i].end());
				vector<int> tmpQosClinet;
				vector<int> location;
				for (int k = 0; k < customerNum; k++) {
					if (qosVec[j][k] == 1) {
						tmpQosClinet.insert(tmpQosClinet.end(), tmpVec[k].begin(), tmpVec[k].end());
						location.push_back(k);
					}
				}
				int break_flag = 0;
				while (total > 0) {
					int maxIdx = -1, maxValue = -1, minValue = 100000000;
					for (int n = 0; n < tmpQosClinet.size(); n++) {
						int tmpValue = tmpQosClinet[n];
						if (maxValue < tmpValue) {
							maxValue = tmpValue;
							maxIdx = n;
						}
						if (tmpValue != 0)
							minValue = min(minValue, tmpValue);
					}
					if (total < minValue)
						break;
					if (total >= maxValue && maxValue > 0) {
						// 找到一维数组在二维数组中的位置
						int tmp1 = maxIdx / maxStreamNum;
						int tmp2 = maxIdx % maxStreamNum;
						int client_id = location[tmp1];
						mMaxTimeBandCustomerSiteStream[i][client_id][j][tmp2] = 1;
						total -= maxValue;
						mSiteMaxBand[j][i] += maxValue;
					}
					tmpQosClinet[maxIdx] = 0;
				}

				for (int k = 0; k < customerNum; k++) {
					if (qosVec[j][k] == 1) {
						for (int n = 0; n < timeStreamNum[i]; n++) {
							if (total >= tmpVec[k][n] && tmpVec[k][n] > 0) {
								mMaxTimeBandCustomerSiteStream[i][k][j][n] = 1;
								total -= tmpVec[k][n];
								mSiteMaxBand[j][i] += tmpVec[k][n];
								tmpVec[k][n] = 0;
							}
						}
					}
				}
			}

		}
	}
	vector<int>outOfUseSiteNum(timeLen, siteNum);
	vector<vector<int>>remaindVec(demandByCustomerVec);
	vector<int> timeFlag(timeLen, 0);
	cout << "begin 5%: " << endl;
	while (totalMaxNum < conformQocSiteNum * maxAFifthNum) {
		//if (totalMaxNum % 1000 == 0 || totalMaxNum + 1 == conformQocSiteNum * maxAFifthNum)
		//cout << "begin 5%: index :" << totalMaxNum << " total: " << conformQocSiteNum * maxAFifthNum << endl;
		demandByCustomerVec.assign(remaindVec.begin(), remaindVec.end());
		int maxTimeIndex = -1, maxSiteIndex = -1, maxUseInAllTime = -1, maxTimeDemandValue = -1;
		double maxTimeDemandAve = -1;
		for (int i = 0; i < timeLen; i++) {
			if (outOfUseSiteNum[i] == 0 || timeFlag[i] == 1)
				continue;
			int curTimeDemandSum = 0;
			for (int k = 0; k < customerNum; k++) {
				curTimeDemandSum += demandByCustomerVec[i][k];
			}
			if (curTimeDemandSum * 1.0 / outOfUseSiteNum[i] > maxTimeDemandAve) {
				maxTimeDemandAve = curTimeDemandSum * 1.0 / outOfUseSiteNum[i];
				maxTimeIndex = i;
			}
		}
		if (maxTimeIndex == -1 || maxTimeDemandAve == 0)
			break;
		for (int j = 0; j < siteNum; j++) {
			if (siteMaxTimeNum[j] >= maxAFifthNum)
				continue;
			int maxBandWidthtmp = min(mSiteMaxBand[j][maxTimeIndex], bandWidthVec[j] - mTimeBand[j][maxTimeIndex]);
			if (maxUseInAllTime <= maxBandWidthtmp) {
				/*if (maxUseInAllTime < maxBandWidthtmp)
				{*/
				maxSiteIndex = j;
				maxUseInAllTime = maxBandWidthtmp;
				//}
				/*else if (maxUseInAllTime == maxBandWidthtmp && siteMaxTimeNum[j] < siteMaxTimeNum[maxSiteIndex]) {
					maxSiteIndex = j;
					maxUseInAllTime = maxBandWidthtmp;
				}*/
			}
		}
		if (maxUseInAllTime == 0 || maxTimeDemandAve == 0)
		{
			if (maxTimeDemandAve != -1)
			{
				timeFlag[maxTimeIndex] = 1;
				continue;
			}
			break;
		}
		outOfUseSiteNum[maxTimeIndex] -= 1;
		maxAFifthIndex[maxSiteIndex][maxTimeIndex] = 1;
		siteMaxTimeNum[maxSiteIndex] += 1;
		totalMaxNum += 1;
		//if (maxUseInAllTime < bandWidthVec[maxSiteIndex])
		//{
		//	//基本不走这一步
		//	for (int k = 0; k < customerNum; k++) {
		//		if (qosVec[maxSiteIndex][k] == 1) {
		//			for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
		//				mTimeBandCustomerSiteStream[maxTimeIndex][k][maxSiteIndex][n] = 1;
		//				remaindVec[maxTimeIndex][k] = demandByCustomerVec[maxTimeIndex][k];
		//				mTimeBand[maxSiteIndex][maxTimeIndex] += demandVec[maxTimeIndex][maxSiteIndex][k];
		//			}
		//		}

		//	}
		//}
		//else if (maxUseInAllTime >= bandWidthVec[maxSiteIndex])
		{
			for (int k = 0; k < customerNum; k++) {
				if (qosVec[maxSiteIndex][k] == 1) {
					for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
						if (mMaxTimeBandCustomerSiteStream[maxTimeIndex][k][maxSiteIndex][n] == 1) {
							mTimeBandCustomerSiteStream[maxTimeIndex][k][maxSiteIndex][n] = 1;
							remaindVec[maxTimeIndex][k] -= demandVec[maxTimeIndex][k][n];
							mTimeBand[maxSiteIndex][maxTimeIndex] += demandVec[maxTimeIndex][k][n];
							demandVec[maxTimeIndex][k][n] = 0;
						}
					}
				}
			}
		}
		int sumDemand = 0, sumRemaind = 0;
		for (int k = 0; k < customerNum; k++) {
			sumDemand += demandByCustomerVec[maxTimeIndex][k];
			sumRemaind += remaindVec[maxTimeIndex][k];
		}
		//if(maxTimeIndex == 0)
		//cout <<"maxTimeIndex: " << maxTimeIndex << " maxSiteIndex: " << maxSiteIndex << " totalMaxNum: " 
		//	<< totalMaxNum<<" sumDemand: "<< sumDemand <<" sumRemaind: " << sumRemaind << " maxUseInAllTime: " << maxUseInAllTime <<
		//	" mSiteMaxBand: " <<mSiteMaxBand[maxSiteIndex][maxTimeIndex]<<
		//	" usage: " << mTimeBand[maxSiteIndex][maxTimeIndex] << endl;
		//将已分配数据从其余节点中剔除
		for (int j = 0; j < siteNum; j++) {
			if (maxAFifthIndex[j][maxTimeIndex] == 1)
				continue;
			int siteDemandSum = 0;
			//与j相连的所有用户的总剩余需求
			mSiteMaxBand[j][maxTimeIndex] = 0;
			for (int k = 0; k < customerNum; k++) {
				if (qosVec[j][k] == 1) {
					siteDemandSum += remaindVec[maxTimeIndex][k];

					for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
						mMaxTimeBandCustomerSiteStream[maxTimeIndex][k][j][n] = 0;
					}
				}
			}
			if (siteDemandSum < bandWidthVec[j]) {
				for (int k = 0; k < customerNum; k++) {
					if (qosVec[j][k] == 1) {
						for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
							if (demandVec[maxTimeIndex][k][n] > 0) {
								mMaxTimeBandCustomerSiteStream[maxTimeIndex][k][j][n] = 1;
								mSiteMaxBand[j][maxTimeIndex] += demandVec[maxTimeIndex][k][n];
							}
						}
					}
				}
			}
			else if (siteDemandSum >= bandWidthVec[j])
			{
				//	int total = bandWidthVec[j];
				int total = min(bandWidthVec[j], siteDemandSum);
				vector<vector<int>> tmpVec(demandVec[maxTimeIndex].begin(), demandVec[maxTimeIndex].end());
				for (int k = 0; k < customerNum; k++) {
					/*if (qosVec[j][k] == 1) {
						for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
							if (total > demandVec[maxTimeIndex][k][n]&&  demandVec[maxTimeIndex][k][n] > 0) {
								mMaxTimeBandCustomerSiteStream[maxTimeIndex][k][j][n] = 1;
								total -= demandVec[maxTimeIndex][k][n];
								mSiteMaxBand[j][maxTimeIndex] += demandVec[maxTimeIndex][k][n];
							}
						}
					}*/
					if (qosVec[j][k] == 1) {
						//vector<int> tmpVec(demandVec[maxTimeIndex][k].begin(), demandVec[maxTimeIndex][k].end());
						int index = 0;
						while (index < 10) {
							index++;
							int maxValue = -1, minValue = 100000000, maxIndex = -1;
							for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
								int tmpValue = tmpVec[k][n];
								if (maxValue < tmpValue) {
									maxValue = tmpValue;
									maxIndex = n;
								}
								if (tmpValue != 0)
									minValue = min(minValue, tmpValue);
							}
							if (total < minValue)
								break;
							if (total >= maxValue && maxValue > 0) {
								mMaxTimeBandCustomerSiteStream[maxTimeIndex][k][j][maxIndex] = 1;
								total -= maxValue;
								mSiteMaxBand[j][maxTimeIndex] += maxValue;

							}
							tmpVec[k][maxIndex] = 0;
						}
						/*for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
							if (total > tmpVec[k][n] && tmpVec[k][n] > 0) {
								mMaxTimeBandCustomerSiteStream[maxTimeIndex][k][j][n] = 1;
								total -= tmpVec[k][n];
								mSiteMaxBand[j][maxTimeIndex] += tmpVec[k][n];
							}
						}*/
					}
				}
				for (int k = 0; k < customerNum; k++) {
					if (qosVec[j][k] == 1) {
						for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
							if (total >= tmpVec[k][n] && tmpVec[k][n] > 0) {
								mMaxTimeBandCustomerSiteStream[maxTimeIndex][k][j][n] = 1;
								total -= tmpVec[k][n];
								mSiteMaxBand[j][maxTimeIndex] += tmpVec[k][n];
								tmpVec[k][n] = 0;
							}
						}
					}
				}
			}
		}
		for (int k = 0; k < customerNum; k++) {
			for (int n = 0; n < timeStreamNum[maxTimeIndex]; n++) {
				mMaxTimeBandCustomerSiteStream[maxTimeIndex][k][maxSiteIndex][n] = 0;
			}
		}
		mSiteMaxBand[maxSiteIndex][maxTimeIndex] = 0;
		//totalRemaindByTime[maxTimeIndex] -= maxUseInAllTime;
	}
	vector<double> remaindMaxAveByCustomer(customerNum, 0);
	vector<int>remaindMaxIndexByCustomer(customerNum, 0);
	int maxSiteNum = -1, maxRemaind = 0;
	for (int i = 0; i < timeLen; i++) {
		int curRemaind = 0, curSiteNum = 0;
		for (int k = 0; k < customerNum; k++) {
			curRemaind += remaindVec[i][k];
		}
		if (maxRemaind < curRemaind)
		{
			maxRemaind = curRemaind;
		}
	}
	cout << maxRemaind << endl;
	int remaindCustomerNum = 0;

	for (int k = 0; k < customerNum; k++) {
		for (int i = 0; i < timeLen; i++) {
			if (remaindVec[i][k] > 0)
			{
				remaindCustomerNum += 1;
				break;
			}
		}
	}
	//	cout << remaindCustomerNum << endl;
	cout << "begin 95% Ave" << endl;
	vector<int> remaindCustomerFlag(customerNum, 0);
	vector<int> tile95SiteMaxDemaind(siteNum, 0);
	while (remaindCustomerNum != 0) {
		int maxAveCustomerIndex = -1;
		int maxAveTimeIndex = -1;
		int maxAveNum = 0;
		int maxDemaindTotalRemaind = 0;
		int maxTimeSiteNum = 0;
		for (int i = 0; i < timeLen; i++) {
			for (int k = 0; k < customerNum; k++) {
				int curTimeSiteNum = 0;
				if (remaindCustomerFlag[k] == 1)
					continue;
				for (int j = 0; j < siteNum; j++) {
					if (qosVec[j][k] == 1 && bandWidthVec[j] > mTimeBand[j][i] && tile95SiteMaxDemaind[j] == 0)
					{
						curTimeSiteNum += 1;
					}
				}
				if (curTimeSiteNum == 0)
					continue;
				if (int(remaindVec[i][k] * 1.0 / curTimeSiteNum) > maxAveNum)
				{
					maxAveCustomerIndex = k;
					maxAveTimeIndex = i;
					maxAveNum = int(remaindVec[i][k] * 1.0 / curTimeSiteNum);
					maxDemaindTotalRemaind = remaindVec[i][k];
					maxTimeSiteNum = curTimeSiteNum;
				}
			}
		}
		if (maxDemaindTotalRemaind == 0)
			break;
		remaindCustomerNum -= 1;
		remaindCustomerFlag[maxAveCustomerIndex] = 1;
		cout << "maxAveTimeIndex: " << maxAveTimeIndex << " maxAveCustomerIndex: " << maxAveCustomerIndex << endl;
		vector<int> maxAveCustomerSiteIndex;
		for (int j = 0; j < siteNum; j++) {
			if (qosVec[j][maxAveCustomerIndex] == 1 && maxAFifthIndex[j][maxAveTimeIndex] == 0 &&
				bandWidthVec[j] - mTimeBand[j][maxAveTimeIndex] > 0 && tile95SiteMaxDemaind[j] == 0) {
				maxAveCustomerSiteIndex.emplace_back(j);
				maxAFifthIndex[j][maxAveTimeIndex] = 1;
				/*if (bandWidthVec[j] - mTimeBand[j][maxAveTimeIndex] > maxAveNum)
					tile95SiteMaxDemaind[j] = maxAveNum + 1;
				else
					tile95SiteMaxDemaind[j] = bandWidthVec[j] - mTimeBand[j][maxAveTimeIndex];*/
			}
		}

		for (int iter = 0; iter < maxAveCustomerSiteIndex.size(); iter++) {
			int siteIndexTmp = maxAveCustomerSiteIndex[iter];
			if (qosVec[siteIndexTmp][maxAveCustomerIndex] == 1 && bandWidthVec[siteIndexTmp] > mTimeBand[siteIndexTmp][maxAveTimeIndex])
			{
				int tmp = 0;
				int total = min(bandWidthVec[siteIndexTmp] - mTimeBand[siteIndexTmp][maxAveTimeIndex], maxAveNum);
				for (int n = 0; n < timeStreamNum[maxAveTimeIndex]; n++) {
					if (total >= demandVec[maxAveTimeIndex][maxAveCustomerIndex][n] && demandVec[maxAveTimeIndex][maxAveCustomerIndex][n] > 0) {
						mTimeBandCustomerSiteStream[maxAveTimeIndex][maxAveCustomerIndex][siteIndexTmp][n] = 1;
						total -= demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
						mTimeBand[siteIndexTmp][maxAveTimeIndex] += demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
						remaindVec[maxAveTimeIndex][maxAveCustomerIndex] -= demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
						maxDemaindTotalRemaind -= demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
						tmp += demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
						demandVec[maxAveTimeIndex][maxAveCustomerIndex][n] = 0;
					}
				}
				tile95SiteMaxDemaind[siteIndexTmp] = tmp;
			}
		}
		if (maxDemaindTotalRemaind != 0)
		{
			for (int iter = maxAveCustomerSiteIndex.size() - 1; iter >= 0; iter--) {
				int siteIndexTmp = maxAveCustomerSiteIndex[iter];
				if (maxDemaindTotalRemaind == 0)
					break;

				if (qosVec[siteIndexTmp][maxAveCustomerIndex] == 1 && bandWidthVec[siteIndexTmp] > mTimeBand[siteIndexTmp][maxAveTimeIndex]) {
					int tmp = 0;
					int total = min(bandWidthVec[siteIndexTmp] - mTimeBand[siteIndexTmp][maxAveTimeIndex], maxDemaindTotalRemaind);
					for (int n = 0; n < timeStreamNum[maxAveTimeIndex]; n++) {
						if (total >= demandVec[maxAveTimeIndex][maxAveCustomerIndex][n] && demandVec[maxAveTimeIndex][maxAveCustomerIndex][n] > 0) {
							total -= demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
							mTimeBandCustomerSiteStream[maxAveTimeIndex][maxAveCustomerIndex][siteIndexTmp][n] = 1;
							mTimeBand[siteIndexTmp][maxAveTimeIndex] += demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
							remaindVec[maxAveTimeIndex][maxAveCustomerIndex] -= demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
							maxDemaindTotalRemaind -= demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
							tmp += demandVec[maxAveTimeIndex][maxAveCustomerIndex][n];
							demandVec[maxAveTimeIndex][maxAveCustomerIndex][n] = 0;
							if (maxDemaindTotalRemaind == 0)
								break;
						}
					}
					tile95SiteMaxDemaind[siteIndexTmp] += tmp;
				}
			}
		}

		for (int i = 0; i < timeLen; i++) {
			int beforeremaindSum = 0;
			for (int k = 0; k < customerNum; k++) {
				beforeremaindSum += remaindVec[i][k];
			}
			int beforedemandSum = 0;
			for (int iter = 0; iter < maxAveCustomerSiteIndex.size(); iter++)
			{
				int siteIndexTmp = maxAveCustomerSiteIndex[iter];
				beforedemandSum += mTimeBand[siteIndexTmp][i];
			}
			//cout << "before remaind: "<< beforeremaindSum <<" before demand: "<< beforedemandSum;

			if (i == maxAveTimeIndex)
				continue;
			vector<int> curTimeRemaindSiteIndex;
			for (int iter = 0; iter < maxAveCustomerSiteIndex.size(); iter++)
			{
				int siteIndexTmp = maxAveCustomerSiteIndex[iter];
				if (qosVec[siteIndexTmp][maxAveCustomerIndex] == 1 && tile95SiteMaxDemaind[siteIndexTmp] != 0 &&
					maxAFifthIndex[siteIndexTmp][i] == 0 && bandWidthVec[siteIndexTmp] > mTimeBand[siteIndexTmp][i])
				{
					curTimeRemaindSiteIndex.emplace_back(siteIndexTmp);
					maxAFifthIndex[siteIndexTmp][i] = 1;
				}
			}
			if (curTimeRemaindSiteIndex.size() == 0)
				continue;
			int curTimeRemaindSiteIndexSize = curTimeRemaindSiteIndex.size();
			while (curTimeRemaindSiteIndexSize != 0)
			{
				//int curTimeMaxAveCustomerRemaind = remaindVec[i][maxAveCustomerIndex] / curTimeRemaindSiteIndexSize;
				int minCurTimeSiteConformNum = 10000, minCurTimeSiteConformIndex = -1, useFlag = -1;
				for (int iter = 0; iter < curTimeRemaindSiteIndex.size(); iter++) {
					int siteIndexTmp = curTimeRemaindSiteIndex[iter];
					if (siteIndexTmp == -1)
						continue;
					int curTimeSiteConformNum = -1;
					for (int k = 0; k < customerNum; k++)
					{
						if (qosVec[siteIndexTmp][k] == 1 && remaindVec[i][k] != 0)
							curTimeSiteConformNum++;
					}
					if (minCurTimeSiteConformNum > curTimeSiteConformNum)
					{
						minCurTimeSiteConformNum = curTimeSiteConformNum;
						minCurTimeSiteConformIndex = siteIndexTmp;
						useFlag = iter;
					}
				}
				curTimeRemaindSiteIndex[useFlag] = -1;
				int curTimeSiteRemaindValue = tile95SiteMaxDemaind[minCurTimeSiteConformIndex]/* - mTimeBandCustomer[i][minCurTimeSiteConformIndex][maxAveCustomerIndex]*/;
				if (curTimeSiteRemaindValue > 0 && remaindVec[i][maxAveCustomerIndex] > 0) {
					int total = min(remaindVec[i][maxAveCustomerIndex], curTimeSiteRemaindValue);
					for (int n = 0; n < timeStreamNum[i]; n++) {
						if (total >= demandVec[i][maxAveCustomerIndex][n] && demandVec[i][maxAveCustomerIndex][n] > 0) {
							mTimeBandCustomerSiteStream[i][maxAveCustomerIndex][minCurTimeSiteConformIndex][n] = 1;
							total -= demandVec[i][maxAveCustomerIndex][n];
							mTimeBand[minCurTimeSiteConformIndex][i] += demandVec[i][maxAveCustomerIndex][n];
							remaindVec[i][maxAveCustomerIndex] -= demandVec[i][maxAveCustomerIndex][n];
							curTimeSiteRemaindValue -= demandVec[i][maxAveCustomerIndex][n];
							demandVec[i][maxAveCustomerIndex][n] = 0;
						}
					}
				}
				if (curTimeSiteRemaindValue > 0) {
					for (int k = 0; k < customerNum; k++) {
						if (qosVec[minCurTimeSiteConformIndex][k] == 1 && remaindVec[i][k] > 0) {
							int total = min(remaindVec[i][k], curTimeSiteRemaindValue);
							for (int n = 0; n < timeStreamNum[i]; n++) {
								if (total >= demandVec[i][k][n] && demandVec[i][k][n] > 0) {
									mTimeBandCustomerSiteStream[i][k][minCurTimeSiteConformIndex][n] = 1;
									total -= demandVec[i][k][n];
									mTimeBand[minCurTimeSiteConformIndex][i] += demandVec[i][k][n];
									remaindVec[i][k] -= demandVec[i][k][n];
									curTimeSiteRemaindValue -= demandVec[i][k][n];
									demandVec[i][k][n] = 0;
								}
							}
						}
					}
				}
				curTimeRemaindSiteIndexSize--;
			}
			int remaindSum = 0;
			for (int k = 0; k < customerNum; k++) {
				remaindSum += remaindVec[i][k];
			}
			int demandSum = 0;
			for (int iter = 0; iter < maxAveCustomerSiteIndex.size(); iter++)
			{
				int siteIndexTmp = maxAveCustomerSiteIndex[iter];
				demandSum += mTimeBand[siteIndexTmp][i];
			}
			if (beforeremaindSum - remaindSum != demandSum - beforedemandSum)
				cout << "error" << endl;
			//cout << " After remaind: " << remaindSum << " demand: " << demandSum -   beforedemandSum<< endl;
		}
	}
	maxSiteNum = -1, maxRemaind = 0;
	for (int i = 0; i < timeLen; i++) {
		int curRemaind = 0, curSiteNum = 0;
		for (int k = 0; k < customerNum; k++) {
			curRemaind += remaindVec[i][k];
		}
		if (maxRemaind < curRemaind)
		{
			maxRemaind = curRemaind;
		}
	}
	cout << maxRemaind << endl;
	cout << "begin baseline" << endl;
	for (int i = 0; i < timeLen; i++) {
		for (int k = 0; k < customerNum; k++) {
			//int demandForCurTimeCustomer = demandByCustomerVec[i][k];
			for (int j = 0; j < siteNum; j++) {
				if (qosVec[j][k] == 1) {
					if (bandWidthVec[j] - mTimeBand[j][i] >= demandByCustomerVec[i][k])
					{
						//mTimeBand[j][i] += demandByCustomerVec[i][k];
						for (int n = 0; n < timeStreamNum[i]; n++) {
							if (demandVec[i][k][n] != 0) {
								mTimeBandCustomerSiteStream[i][k][j][n] = 1;
								mTimeBand[j][i] += demandVec[i][k][n];
								demandVec[i][k][n] = 0;
							}
						}
						demandByCustomerVec[i][k] = 0;
						break;
					}
					else {
						//int maxStream = -1,maxStreamIndex = -1;
						int remaind = bandWidthVec[j] - mTimeBand[j][i];
						for (int n = 0; n < timeStreamNum[i]; n++) {
							int demandForCurCustomerStream = demandVec[i][k][n];
							if (demandForCurCustomerStream != 0 && demandForCurCustomerStream < remaind) {
								mTimeBandCustomerSiteStream[i][k][j][n] = 1;
								remaind -= demandForCurCustomerStream;
								mTimeBand[j][i] += demandForCurCustomerStream;
								demandVec[i][k][n] = 0;
								demandByCustomerVec[i][k] -= demandForCurCustomerStream;
							}
						}
					}
				}
			}
		}
	}

	double curScore = 0;
	//vector<int> tile95ValueVec(bandWidthVec.size(), 0);
	curScore = getScore(mTimeBand, bandWidthVec, tile95, baseCost);
	cout << "Score: " << curScore << endl;

	cout << "begin write results" << endl;
	ofstream outputFile;
	outputFile.open(outputFilename, ios::out);
	for (int i = 0; i < timeLen; i++) {
		for (int k = 0; k < customerNum; k++) {
			string solutionstr = "";
			solutionstr += customerID[k] + ":";
			for (int j = 0; j < siteNum; j++) {
				string nowSiteStr = "";
				for (int n = 0; n < timeStreamNum[i]; n++) {
					if (mTimeBandCustomerSiteStream[i][k][j][n] == 1) {
						nowSiteStr += "," + timeStreamID[i][n];
					}
				}
				if (nowSiteStr != "")
					solutionstr += "<" + siteNameVec[j] + nowSiteStr + ">,";
			}
			if (solutionstr[solutionstr.size() - 1] == ',')
				solutionstr = solutionstr.substr(0, solutionstr.size() - 1);
			solutionstr += '\n';
			outputFile << solutionstr;
		}
	}
	end_time = clock();
	printf("time = %lf", double(end_time - start_time) / CLOCKS_PER_SEC);
	return 0;
}

