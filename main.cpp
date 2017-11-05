#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
using namespace std;

#define MATCH 4
#define MISMATCH -3
#define GAP -2

int scoring_function(char a, char b) {
	if (a == b)
		return MATCH;
	else
		return MISMATCH;
}

int get_minimum_distance_index(string &reference_string, string &genome_string,
	int vrs[], int vgs[],
	map<string, int> &triplet_indices) {
	int rslti = reference_string.length() - 2;
	int gslti = genome_string.length() - 2;
	int min_distance, distance = 0, min_window_index = 0;

	for (int i = 0; i < rslti; i++) {
		string current_triplet = genome_string.substr(i, 3);
		vgs[triplet_indices[current_triplet]]++;
	}

	for (int i = 0; i < 64; i++) {
		distance += abs(vrs[i] - vgs[i]);
	}
	min_distance = distance;

	for (int i = rslti; i < gslti; i++) {
		string leaving_triplet, entering_triplet;
		leaving_triplet = genome_string.substr(i - rslti, 3);
		entering_triplet = genome_string.substr(i, 3);

		distance -= abs(vrs[triplet_indices[leaving_triplet]] -
			vgs[triplet_indices[leaving_triplet]]);
		distance -= abs(vrs[triplet_indices[entering_triplet]] -
			vgs[triplet_indices[entering_triplet]]);

		vgs[triplet_indices[leaving_triplet]]--;
		vgs[triplet_indices[entering_triplet]]++;

		distance += abs(vrs[triplet_indices[leaving_triplet]] -
			vgs[triplet_indices[leaving_triplet]]);
		distance += abs(vrs[triplet_indices[entering_triplet]] -
			vgs[triplet_indices[entering_triplet]]);

		if (distance < min_distance) {
			min_window_index = i - rslti + 1;
			min_distance = distance;
		}
	}

	return min_window_index;
}

void init_score(int **matrix, int height, int width) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (j == 0) {
				matrix[i][j] = 0;
			}
			else if (i == 0) {
				matrix[i][j] = 0;
			}
			else {
				matrix[i][j] = -1;
			}
		}
	}
	return;
}

void init_move(char **matrix, int height, int width) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (j == 0) {
				matrix[i][j] = 'S';
			}
			else if (i == 0) {
				matrix[i][j] = 'S';
			}
			else {
				matrix[i][j] = 'Z';
			}
		}
	}
	return;
}

char move_direction(int score, int diag, int left, int up) {
	if (score == diag)
		return 'D';
	else if (score == up)
		return 'U';
	else if (score == left)
		return 'L';
	else
		return 'S';
}

int local_alignment(string m, string n) {

	int width = m.length() + 1;
	int height = n.length() + 1;
	int **score_matrix = new int *[height];
	for (int i = 0; i < height; i++) {
		score_matrix[i] = new int[width];
	}
	char **move_matrix = new char *[height];
	for (int i = 0; i < height; i++) {
		move_matrix[i] = new char[width];
	}

	int max_x = 0, max_y = 0;

	init_score(score_matrix, height, width);
	init_move(move_matrix, height, width);

	cout << "width height: " << width << " " << height << endl;

	for (int i = 1; i < height; i++) {
		for (int j = 1; j < width; j++) {
			int score = max(max(max(0, score_matrix[i - 1][j - 1] +
				scoring_function(m[j - 1], n[i - 1])),
				score_matrix[i][j - 1] + GAP),
				score_matrix[i - 1][j] + GAP);
			score_matrix[i][j] = score;
			char dir = move_direction(
				score,
				score_matrix[i - 1][j - 1] + scoring_function(m[j - 1], n[i - 1]),
				score_matrix[i][j - 1] + GAP, score_matrix[i - 1][j] + GAP);
			move_matrix[i][j] = dir;
			if (score > score_matrix[max_y][max_x]) {
				max_y = i;
				max_x = j;
			}
		}
	}

	cout << endl;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%2d ", score_matrix[i][j]);
			// cout << score_matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%2c ", move_matrix[i][j]);
			// cout << score_matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	cout << max_x << " " << max_y << endl;
	string a, b, c;
	a = b = c = "";
	while (move_matrix[max_y][max_x] != 'S') {
		if (move_matrix[max_y][max_x] == 'D') {
			cout << "Adding " << m[max_x - 1] << " to A  ";
			cout << "Adding " << n[max_y - 1] << " to B" << endl;
			a.insert(0, 1, m[max_x - 1]);
			b.insert(0, 1, n[max_y - 1]);
			if (n[max_y - 1] == m[max_x - 1])
				c.insert(0, 1, '|');
			else
				c.insert(0, 1, '*');
			max_y--;
			max_x--;
		}
		else if (move_matrix[max_y][max_x] == 'L') {
			cout << "Adding " << m[max_x - 1] << " to A" << endl;
			b.insert(0, 1, '-');
			a.insert(0, 1, m[max_x - 1]);
			c.insert(0, 1, ' ');
			max_x--;
		}
		else if (move_matrix[max_y][max_x] == 'U') {
			cout << "               Adding " << n[max_y - 2] << " to B" << endl;
			b.insert(0, 1, n[max_y - 1]);
			a.insert(0, 1, '-');
			c.insert(0, 1, ' ');
			max_y--;
		}
		// cout << a << endl << c << endl << b << endl << endl;
	}
	cout << a << endl << c << endl << b << endl;

	return score_matrix[max_y][max_x];
}

int main(int argc, char **argv) {
	map<string, int> triplet_indices{
		{ "AAA", 0 },{ "AAT", 1 },{ "AAC", 2 },{ "AAG", 3 },{ "ATA", 4 },
		{ "ATT", 5 },{ "ATC", 6 },{ "ATG", 7 },{ "ACA", 8 },{ "ACT", 9 },
		{ "ACC", 10 },{ "ACG", 11 },{ "AGA", 12 },{ "AGT", 13 },{ "AGC", 14 },
		{ "AGG", 15 },{ "TAA", 16 },{ "TAT", 17 },{ "TAC", 18 },{ "TAG", 19 },
		{ "TTA", 20 },{ "TTT", 21 },{ "TTC", 22 },{ "TTG", 23 },{ "TCA", 24 },
		{ "TCT", 25 },{ "TCC", 26 },{ "TCG", 27 },{ "TGA", 28 },{ "TGT", 29 },
		{ "TGC", 30 },{ "TGG", 31 },{ "CAA", 32 },{ "CAT", 33 },{ "CAC", 34 },
		{ "CAG", 35 },{ "CTA", 36 },{ "CTT", 37 },{ "CTC", 38 },{ "CTG", 39 },
		{ "CCA", 40 },{ "CCT", 41 },{ "CCC", 42 },{ "CCG", 43 },{ "CGA", 44 },
		{ "CGT", 45 },{ "CGC", 46 },{ "CGG", 47 },{ "GAA", 48 },{ "GAT", 49 },
		{ "GAC", 50 },{ "GAG", 51 },{ "GTA", 52 },{ "GTT", 53 },{ "GTC", 54 },
		{ "GTG", 55 },{ "GCA", 56 },{ "GCT", 57 },{ "GCC", 58 },{ "GCG", 59 },
		{ "GGA", 60 },{ "GGT", 61 },{ "GGC", 62 },{ "GGG", 63 } };
	
	ifstream g1,g2,rs;
	g1.open("g1.txt");
	g2.open("g2.txt");
	
	string reference_string = "ATCCC";
	string genome1;
	string genome2;

	g1 >> genome1;
	g2 >> genome2;

	cout << genome1 << endl << endl << genome2 << endl << endl;

	int vrs[64] = { 0 };
	int vg1[64] = { 0 };
	int vg2[64] = { 0 };
	int min_window_index_1 = 0, min_window_index_2 = 0;

	for (int i = 0; i < (int)reference_string.length() - 2; i++) {
		string current_triplet = reference_string.substr(i, 3);
		vrs[triplet_indices[current_triplet]]++;
	}

	min_window_index_1 = get_minimum_distance_index(reference_string, genome1,
		vrs, vg1, triplet_indices);
	min_window_index_2 = get_minimum_distance_index(reference_string, genome2,
		vrs, vg2, triplet_indices);

	string genome1_match =
		genome1.substr(min_window_index_1, reference_string.length());
	string genome2_match =
		genome2.substr(min_window_index_2, reference_string.length());

	cout << "Min Window Indices: " << min_window_index_1 << " "
		<< min_window_index_2 << endl;
	cout << "Mat string 1: " << genome1_match << "Mat String 2: " << genome2_match
		<< endl;

	cout << genome1 << "\n\n" << genome2 << endl;
	cout << local_alignment(genome1, genome2) << endl;

	system("pause");
}
