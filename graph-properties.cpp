#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const int width = 8;
const int length = 4;
const int height = 4;
const int n = width*length*height;
vector< vector<bool> > twist = {{false, false}, {false, false}, {false, false}};
vector<int> adj[width*length*height];

int get_index (int x, int y, int z) {
    return x*length*height + y*height + z;
}

int get_x (int id) {
	return id / (length*height);
}

int get_y (int id) {
	return (id / height) % length;
}

int get_z (int id) {
	return id % height;
}

int dist[n];
int dist_x[n];
int dist_y[n];
int dist_z[n];
void calc_dist (int s_id) {
	for (int i=0; i<n; i++) {
		dist[i] = dist_x[i] = dist_y[i] = dist_z[i] = -1;
	}
	dist[s_id] = 0;
	queue<int> ord;
	ord.push(s_id);
	while (!ord.empty()) {
		int cur_id = ord.front();

		for (int i=0; i<n; i++) {
			if (get_x(i) == get_x(cur_id) && dist_x[i] == -1) dist_x[i] = dist[cur_id];
			if (get_y(i) == get_y(cur_id) && dist_y[i] == -1) dist_y[i] = dist[cur_id];
			if (get_z(i) == get_z(cur_id) && dist_z[i] == -1) dist_z[i] = dist[cur_id];
		}

		ord.pop();
		for (auto i: adj[cur_id]) {
			if (dist[i] == -1) {
				dist[i] = dist[cur_id]+1;
				ord.push(i);
			}
		}
	}
}

double calc_avg_dist () {
	for (int i=0; i<n; i++) {
		adj[i].clear();
	}

	for (int x=0; x<width; x++) {
		for (int y=0; y<length; y++) {
			for (int z=0; z<height; z++) {
				int id = get_index(x, y, z);

				int nx, ny, nz;

				nx = (x+1) % width;
				ny = y;
				if (twist[0][0] && nx == 0) {
				    ny = (ny + length/2) % length;
				}
				nz = z;
				if (twist[0][1] && nx == 0) {
				    nz = (nz + height/2) % height;
				}
				int nx_id = get_index(nx, ny, nz);

				ny = (y+1) % length;
				nx = x;
				if (twist[1][0] && ny == 0) {
				    nx = (nx + width/2) % width;
				}
				nz = z;
				if (twist[1][1] && ny == 0) {
				    nz = (nz + height/2) % height;
				}
				int ny_id = get_index(nx, ny, nz);

				nz = (z+1) % height;
				nx = x;
				if (twist[2][0] && nz == 0) {
				    nx = (nx + width/2) % width;
				}
				ny = y;
				if (twist[2][1] && nz == 0) {
				    ny = (ny + length/2) % length;
				}
				int nz_id = get_index(nx, ny, nz);

				adj[id].push_back(nx_id);
				adj[id].push_back(ny_id);
				adj[id].push_back(nz_id);
				adj[nx_id].push_back(id);
				adj[ny_id].push_back(id);
				adj[nz_id].push_back(id);
			}
		}
	}

	double total_dist = 0;
	double total_dist_x = 0;
	double total_dist_y = 0;
	double total_dist_z = 0;
	int diameter = 0;
	for (int i=0; i<n; i++) {
		calc_dist(i);
		for (int j=0; j<n; j++) {
			total_dist += dist[j];
			total_dist_x += dist_x[j];
			total_dist_y += dist_y[j];
			total_dist_z += dist_z[j];
			diameter = max(diameter, dist[j]);
		}
	}

	double pairs = n * n;
	double avg_dist = total_dist / pairs;
	return avg_dist;

	/*
	double pairs = n * n;
	double avg_dist = total_dist / pairs;
	double avg_x_dist = total_dist_x / pairs;
	double avg_y_dist = total_dist_y / pairs;
	double avg_z_dist = total_dist_z / pairs;
	double link_util = 1.0/3.0 * avg_dist / max(avg_x_dist, max(avg_y_dist, avg_z_dist));

	cout << "Diameter = " << diameter << endl;
	cout << "Average distance = " << avg_dist << endl;
	cout << "Average x distance = " << avg_x_dist << endl;
	cout << "Average y distance = " << avg_y_dist << endl;
	cout << "Average z distance = " << avg_z_dist << endl;
	cout << "Link utilization = " << link_util << endl;
	*/
}

int main () {
	twist[1][0] = true;
	cout << calc_avg_dist() << endl;

	twist[1][0] = false;
	twist[0][0] = true;
	cout << calc_avg_dist() << endl;

	twist[0][0] = false;
	twist[1][1] = true;
	cout << calc_avg_dist() << endl;
	/*
	for (int i=0; i<64; i++) {
		int twist_ind = i;
		for (int j=0; j<3; j++) {
			twist[j][0] = twist_ind%2;
			twist_ind /= 2;
			twist[j][1] = twist_ind%2;
			twist_ind /= 2;
		}

		cout << calc_avg_dist() << endl;
	}
	*/
}