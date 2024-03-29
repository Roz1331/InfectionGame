#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>

using namespace BOARD;

namespace INFECTION
{
	struct infection
	{
		board state;
		int winner;
		int curPlayer;
		int firstMove;
		int maxDepth = 5;
		string bestFrom;
		string bestTo;

		infection(int st[6][6], int pl = 1, int fm = 1)
		{
			state = board(st);
			curPlayer = pl;
			firstMove = fm;
		}

		void initArray(int copyfrom[6][6], int copyto[6][6])
		{
			for (int i = 0; i < 6; i++)
				for (int j = 0; j < 6; j++)
					copyto[i][j] = copyfrom[i][j];
		}

		void infectOpponent(int field[6][6], int x, int y, int player)
		{
			for (int i = 0; i < 6; i++)
				for (int j = 0; j < 6; j++)
					if (abs(i - x) <= 1 && abs(y - j) <= 1 && field[i][j] != player && field[i][j] != 0)
						field[i][j] = player;
		}

		vector<board> moves(board st, int player)
		{
			vector<board> result;
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 6; j++)
				{
					// ��� "������" �������� �� �����
					if (st.state[i][j] == player)
					{
						for (int ii = 0; ii < 6; ii++)
						{
							for (int jj = 0; jj < 6; jj++)
							{
								if (st.state[ii][jj] == 0)
								{
									bestFrom = "";
									bestTo = "";

									char charFrom = j + 'a';
									char intFrom = i + '0' + 1;

									bestFrom += charFrom;
									bestFrom += intFrom;

									char charTo = jj + 'a';
									char intTo = ii + '0' + 1;

									bestTo += charTo;
									bestTo += intTo;

									if (abs(i - ii) <= 1 && abs(j - jj) <= 1) // ������������ ������ ����
									{

										int tmp[6][6];
										initArray(st.state, tmp);

										tmp[ii][jj] = player;

										// "��������" ������ ����������
										infectOpponent(tmp, ii, jj, player);
										board brd = board(tmp);
										brd.posFrom = bestFrom;
										brd.posTo = bestTo;
										result.push_back(brd);
									}
									else if (abs(i - ii) <= 2 && abs(j - jj) <= 2) //������������ ������ ����
									{
										int tmp[6][6];
										initArray(st.state, tmp);

										tmp[i][j] = 0;
										tmp[ii][jj] = player;

										// "��������" ������ ����������
										infectOpponent(tmp, ii, jj, player);
										board brd = board(tmp);
										brd.posFrom = bestFrom;
										brd.posTo = bestTo;
										result.push_back(brd);
									}
								}
							}
						}
					}
				}
			}
			return result;
		}

		tuple<board, int> findBestMoveForFirstPlayer(board brd, int player = 1, int depth = 4, int currentStep = 1, int alpha = INT_MIN, int beta = INT_MAX)
		{
			if (depth == 0)
				return tuple<board, int>(brd, brd.heur);

			int cur;
			int localBest = INT_MAX;
			board localBestBoard = state;

			if (currentStep % 2 == 0) // ������ ���� - ���� min, ��������� beta
			{
				for (board move : moves(brd, player))
				{
					cur = get<1>(findBestMoveForFirstPlayer(move.state, player == 1 ? 2 : 1, depth - 1, currentStep + 1, alpha, beta));

					beta = min(cur, beta);

					if (cur < localBest)
					{
						localBest = cur;
						localBestBoard = move;
					}

					if (beta < alpha) // ���������
						break;
				}
			}
			else // �������� ���� - ���� max, ��������� alpha
			{
				localBest = INT_MIN;
				for (board move : moves(brd, player))
				{
					cur = get<1>(findBestMoveForFirstPlayer(move.state, player == 1 ? 2 : 1, depth - 1, currentStep + 1, alpha, beta));

					alpha = max(alpha, cur);

					if (cur > localBest)
					{
						localBest = cur;
						localBestBoard = move;
					}

					if (beta < alpha) // ���������
						break;
				}
			}

			return make_tuple(localBestBoard, localBest);
		}

		tuple<board, int> findBestMoveForSecondPlayer(board brd, int player = 2, int depth = 4, int currentStep = 1, int alpha = INT_MIN, int beta = INT_MAX)
		{
			if (depth == 0)
				return tuple<board, int>(brd, -brd.heur);

			int cur;
			int localBest = INT_MAX;
			board localBestBoard = state;

			if (currentStep % 2 == 0) // ������ ���� - ���� min, ��������� beta
			{
				for (board move : moves(brd, player))
				{
					cur = get<1>(findBestMoveForSecondPlayer(move.state, player == 1 ? 2 : 1, depth - 1, currentStep + 1, alpha, beta));
					
					beta = min(cur, beta);

					if (cur < localBest)
					{
						localBest = cur;
						localBestBoard = move;
					}

					if (beta < alpha) // ���������
						break;
				}
			}
			else // �������� ���� - ���� max, ��������� alpha
			{
				localBest = INT_MIN;
				for (board move : moves(brd, player))
				{
					cur = get<1>(findBestMoveForSecondPlayer(move.state, player == 1 ? 2 : 1, depth - 1, currentStep + 1, alpha, beta));

					alpha = max(alpha, cur);

					if (cur > localBest)
					{
						localBest = cur;
						localBestBoard = move;
					}

					if (beta < alpha) // ���������
						break;
				}
			}
			return make_tuple(localBestBoard, localBest);
		}

		void makeBestMove(int player, int d)
		{
			if (curPlayer == 1)
				state = get<0>(findBestMoveForFirstPlayer(state, 1, d));
			else if(curPlayer == 2) state = get<0>(findBestMoveForSecondPlayer(state, 2, d));
		}
		
		bool isGameOver()
		{
			if (curPlayer == 1 && !moves(state, 2).empty())
			{
				cout << "===========================\n";
				return false;
			}
			int score1 = 0;
			int score2 = 0;
			int emptyCells = 0;

			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 6; j++)
				{
					if (state.state[i][j] == 1)
						score1++;
					else if (state.state[i][j] == 2)
						score2++;
					else emptyCells++;
				}
			}

			// ���������� ����������

			if (score1 == 0)
			{
				cout << "---- winner = 2 ----\n";
				winner = 2;
				return true;
			}
			else if (score2 == 0)
			{
				cout << "---- winner = 1 ----\n";
				winner = 1;
				return true;
			}
			else if (emptyCells == 0 || ((moves(state, 1).size() == 0 && moves(state, 2).size() == 0)))
			{
				cout << "-----------------------------------------\n";
				print();
				if (state.heuristic(curPlayer) > 0)
				{
					cout << "heur == " << state.heur << endl;
					cout << "----| winner = 1 |----\n";
					winner = 1;
				}
				else if (state.heuristic(curPlayer) < 0)
				{
					cout << "----| winner = 2 |----\n";
					winner = 2;
				}
				else
				{
					cout << "----| winner = 0 |----\n";
					winner = 0;
				} // draw
				return true;
			}
		
			
			return false;
		}

		string toLowerCase(string str)
		{
			transform(str.begin(), str.end(), str.begin(),
				[](unsigned char c) { return tolower(c); });
			return str;
		}

		int cointPoints(int pl)
		{
			int res = 0;

			for (int i = 0; i < 6; i++)
				for (int j = 0; j < 6; j++)
					if (state.state[i][j] == pl)
						res++;
			return res;
		}

		void playerMove()
		{
			string input1, input2;
			bool startFlag = true;

			// ��������� ���� � �������
			while (startFlag)
			{
				cin >> input1 >> input2;

				if (input1.size() != 2 || input2.size() != 2)
				{
					cout << "invalid input! try again" << endl;
					continue;
				}

				input1 = toLowerCase(input1);
				input2 = toLowerCase(input2);
				
				if (!isalpha(input1[0]) || !isalpha(input2[0]) || !isdigit(input1[1]) || !isdigit(input2[1]))
				{
					cout << "invalid input! try again" << endl;
					continue;
				}

				int charFrom = input1[0] - 'a';
				int intFrom = input1[1] - '0' - 1;
				int charTo = input2[0] - 'a';
				int intTo = input2[1] - '0' - 1;

				// ���������, ��������� �� ����� ���
				if(state.state[intFrom][charFrom] != curPlayer || state.state[intTo][charTo] != 0)
				{
					cout << "invalid input! try again1" << endl;
					continue;
				}
				if (abs(charFrom - charTo) > 2 || abs(intFrom - intTo) > 2)
				{
					cout << "invalid input! try again2" << endl;
					continue;
				}
				if (charFrom > 5 || intFrom > 5 || charTo > 5 || intTo > 5) 
				{
					cout << "invalid input! try again3" << endl;
					continue;
				}

				if (abs(charFrom - charTo) <= 1 && abs(intFrom - intTo) <= 1)
				{
					state.state[intTo][charTo] = curPlayer;
					// infecting
					for (int i = 0; i < 6; i++)
					{
						for (int j = 0; j < 6; j++)
						{
							if (abs(i - intTo) <= 1 && abs(j - charTo) <= 1 && state.state[i][j] != 0)
								state.state[i][j] = curPlayer;
						}
					}
				}
				else if (abs(charFrom - charTo) <= 2 && abs(intFrom - intTo) <= 2)
				{
					state.state[intFrom][charFrom] = 0;
					state.state[intTo][charTo] = curPlayer;

					// infecting
					for (int i = 0; i < 6; i++)
					{
						for (int j = 0; j < 6; j++)
						{
							if (abs(i - intTo) <= 1 && abs(j - charTo) <= 1 && state.state[i][j] != 0)
								state.state[i][j] = curPlayer;
						}
					}
				}

				startFlag = false;
			}
		}

		void getMoveFromAnotherBot()
		{
			string input1, input2;
			bool startFlag = true;

			int opponent = curPlayer == 1 ? 2 : 1;

			// ��������� ���� � �������
			while (startFlag)
			{
				cin >> input1 >> input2;

				if (input1.size() != 2 || input2.size() != 2)
				{
					cout << "invalid input! try again" << endl;
					continue;
				}

				input1 = toLowerCase(input1);
				input2 = toLowerCase(input2);

				if (!isalpha(input1[0]) || !isalpha(input2[0]) || !isdigit(input1[1]) || !isdigit(input2[1]))
				{
					cout << "invalid input! try again" << endl;
					continue;
				}

				int charFrom = input1[0] - 'a';
				int intFrom = input1[1] - '0' - 1;
				int charTo = input2[0] - 'a';
				int intTo = input2[1] - '0' - 1;

				// ���������, ��������� �� ����� ���
				if (state.state[intFrom][charFrom] != opponent || state.state[intTo][charTo] != 0)
				{
					cout << "invalid input! try again1" << endl;
					continue;
				}
				if (abs(charFrom - charTo) > 2 || abs(intFrom - intTo) > 2)
				{
					cout << "invalid input! try again2" << endl;
					continue;
				}
				if (charFrom > 5 || intFrom > 5 || charTo > 5 || intTo > 5)
				{
					cout << "invalid input! try again3" << endl;
					continue;
				}

				makeMoveOfOpponent(opponent, charFrom, intFrom, charTo, intTo);

				startFlag = false;
			}
		}

		void makeMoveOfOpponent(int opponent, int charFrom, int intFrom, int charTo, int intTo)
		{
			if (abs(charFrom - charTo) <= 1 && abs(intFrom - intTo) <= 1)
			{
				state.state[intTo][charTo] = opponent;
			}
			else if (abs(charFrom - charTo) <= 2 && abs(intFrom - intTo) <= 2)
			{
				state.state[intFrom][charFrom] = 0;
				state.state[intTo][charTo] = opponent;
			}

			for (int i = 0; i < 6; i++)
				for (int j = 0; j < 6; j++)
					if (abs(i - intTo) <= 1 && abs(charTo - j) <= 1 && state.state[i][j] != opponent && state.state[i][j] != 0)
						state.state[i][j] = opponent;
		}
		
		void startGame(int pl = 1, int fm = 1)
		{
			curPlayer = pl;
			firstMove = fm;

			print();

			if (firstMove != curPlayer)
			{
				if (!moves(state, 2).empty())
				{
					/*makeBestMove(curPlayer == 1 ? 2 : 1, maxDepth);*/
					makeBestMove(2, maxDepth);
					cout << "opponent move" << endl;
					print();
				}
			}

			while (!isGameOver())
			{
				if (!moves(state, 1).empty())
				{
					cout << "your turn" << endl;
					playerMove();
					print();
				}
				if (!moves(state, 2).empty())
				{
					cout << "opponent move" << endl;
					makeBestMove(2, maxDepth);
					print();
				}
			}

			switch (winner)
			{
			case 1:
				cout << "Well done! You won!" << endl;
				break;
			case 2:
				cout << "Sorry, you lost(((" << endl;
				break;
			case 0:
				cout << "Not bad. Draw!" << endl;
				break;
			}
		}

		void botBattle(int depth1 = 4, int depth2 = 4, int turn = 1)
		{
			firstMove = turn;

			print();

			if (firstMove != curPlayer)
			{
				if (!moves(state, 2).empty())
				{
					makeBestMove(curPlayer == 1 ? 2 : 1, depth1);
					cout << "player 2 move " << state.posFrom << " " << state.posTo << endl;
					print();
				}
			}

			while (!isGameOver())
			{
				if (!moves(state, 1).empty())
				{
					makeBestMove(curPlayer, depth1);
					cout << "player 1 move " << state.posFrom << " " << state.posTo << endl;
					print();
				}
				if (!moves(state, 2).empty())
				{
					makeBestMove(2, depth2);
					cout << "player 2 move " << state.posFrom << " " << state.posTo << endl;
					print();
				}
			}

			switch (winner)
			{
			case 1:
				cout << "Player 1 won" << endl;
				break;
			case 2:
				cout << "Player 2 won" << endl;
				break;
			case 0:
				cout << "Not bad. Draw!" << endl;
				break;
			}
		}

		void playWithBot(int fm, int cp)
		{
			curPlayer = cp;
			firstMove = fm;

			print();
			bool isFirstMove = true;

			while (!isGameOver())
			{
				if (curPlayer == 1)
				{
					if (!isFirstMove)
					{
						getMoveFromAnotherBot();
					}
					vector<board> vv = moves(state, 1);
					if (!vv.empty())
					{
						if (vv.size() <= 4) maxDepth = vv.size();
						makeBestMove(1, maxDepth);
						cerr << state.posFrom << " " << state.posTo << endl;
						cout << "player 1 move " << state.posFrom << " " << state.posTo << endl;
						print();
					}
				}
				if (curPlayer == 2)
				{
					getMoveFromAnotherBot();
					vector<board> vv = moves(state, 2);
					if (!vv.empty())
					{
						if (vv.size() <= 4) maxDepth = vv.size();
						makeBestMove(2, maxDepth);
						cerr << state.posFrom << " " << state.posTo << endl;
						cout << "player 2 move " << state.posFrom << " " << state.posTo << endl;
						print();
					}
				}
				isFirstMove = false;

			}
		}
		
		void print()
		{
			cout << "   ";
			for (int i = 0; i < 6; i++)
				cout << (char)('a' + i) << ' ';
			cout << endl << "  ";
			for (int i = 0; i < 6; i++)
				cout << "__";
			cout << endl;
			for (int i = 0; i < 6; i++)
			{
				cout << i + 1 << "| ";
				for (int j = 0; j < 6; j++)
					cout << state.state[i][j] << ' ';
				cout << endl;
			}
			cout << endl;
		}

		void printBoard(int st[6][6])
		{
			cout << "   ";
			for (int i = 0; i < 6; i++)
				cout << (char)('a' + i) << ' ';
			cout << endl << "  ";
			for (int i = 0; i < 6; i++)
				cout << "__";
			cout << endl;
			for (int i = 0; i < 6; i++)
			{
				cout << i + 1 << "| ";
				for (int j = 0; j < 6; j++)
					cout << st[i][j] << ' ';
				cout << endl;
			}
			cout << endl;
		}
	};
}
