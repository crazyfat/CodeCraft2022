import numpy as np


class DataLoader:
    def __init__(self, path):
        config = np.loadtxt(path + 'config.ini', dtype=str, delimiter=',')
        limit = config[1].split('=', 1)[1]
        self.qos_limit = int(limit)
        self.demand, self.n_user, self.n_clock = load_demand(path + 'demand.csv', ',')
        self.qos_matrix, self.n_site = load_qos(path + 'qos.csv', ',', self.qos_limit)
        self.site_width = load_width(path + 'site_bandwidth.csv', ',')
        tmp = np.loadtxt(path + 'demand.csv', dtype=str, delimiter=',')
        a = tmp[0, :]
        self.id2user = a[1:]

        tmp2 = np.loadtxt(path + 'site_bandwidth.csv', dtype=str,  delimiter=',')
        b = tmp2[:, 0]
        self.id2site = b[1:]

        alldata = np.loadtxt(path + 'qos.csv', dtype=str,  delimiter=',')
        a = alldata[0, :]
        b = alldata[:, 0]
        qos_ulist = a[1:]
        qos_slist = b[1:]

        demand_locate = np.where(self.id2user == qos_ulist[0])
        for i in range(1, len(qos_ulist)):
            demand_locate = np.concatenate([demand_locate, np.where(self.id2user == qos_ulist[i])], axis=-1)
        band_locate = np.where(self.id2site == qos_slist[0])
        for i in range(1, len(qos_slist)):
            band_locate = np.concatenate([band_locate, np.where(self.id2site == qos_slist[i])], axis=-1)

        demand_locate = demand_locate.squeeze(axis=0)
        band_locate = band_locate.squeeze(axis=0)

        self.qos_matrix = self.qos_matrix[:, demand_locate]
        self.qos_matrix = self.qos_matrix[band_locate, :]



class Site:
    def __init__(self, qos_matrix, num, width, n_clock):
        self.qos_matrix, self.num, self.retain = qos_matrix, num, width  # qos矩阵每行为site，每列为user
        self.occupied = np.zeros_like(self.retain)
        self.max = self.retain
        self.occupiedByTime = np.zeros((self.num, n_clock))

    def up_retain(self, occupied):
        self.retain = self.max - occupied


class User:
    def __init__(self, demand, matrix, n_user):
        self.demand, self.matrix, self.n_user = demand, matrix, n_user

    def up_qos(self, matrix):
        self.qos_matrix = matrix.T


def strip_first_col(fname, delimiter=None):
    with open(fname, 'r') as fin:
        for idx, line in enumerate(fin):
            if idx == 0:
                continue
            try:
                yield line.split(delimiter, 1)[1]
            except IndexError:
                continue


def load_demand(fname, d):
    demand = np.loadtxt(strip_first_col(fname, d), delimiter=d)
    u_num = np.shape(demand)[1]
    t_num = np.shape(demand)[0]
    return demand, u_num, t_num


def load_qos(fname, d, limit):
    qos, _, site_num = load_demand(fname, d)
    qos_matrix = np.where(qos <= limit, 1, 0)
    return qos_matrix, site_num


def load_width(fname, d):
    width = np.loadtxt(strip_first_col(fname, d), delimiter=d)
    return width
