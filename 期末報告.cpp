#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// �d�P���c�Grank 0~12 �N�� 2 ~ Ace�F
//suit 0��3 ���O�N�� clubs, diamonds, hearts, spades
struct Card {
    int rank;  // 0 -> 2, ... , 12 -> Ace
    int suit;  // 0: clubs, 1: diamonds, 2: hearts, 3: spades
};

// �洫��i�P
void swapCards(Card &a, Card &b) {
    Card temp = a;
    a = b;
    b = temp;
}

// ��l�� 52 �i�P
void initDeck(Card deck[52]) {
    int index = 0;
    for (int r = 0; r < 13; r++) {
        for (int s = 0; s < 4; s++) {
            deck[index].rank = r;
            deck[index].suit = s;
            index++;
        }
    }
}

// Fisher�VYates �~�P
void shuffleDeck(Card deck[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swapCards(deck[i], deck[j]);
    }
}

// ²�ƪ� 7 �i�P��P�����禡�G���ƶV����ܵP���V�j�C
// �����̧Ǭ��G
// 8: ���l�P��, 7: �K��, 6: ��Ī, 5: �P��, 4: ���l,
// 3: �T��, 2: ���, 1: �@��, 0: ���P
int evaluateHand(Card hand[7]) {
    // �ƻs��P�è� rank �����Ƨ�
    Card temp[7];
    for (int i = 0; i < 7; i++) {
        temp[i] = hand[i];
    }
    for (int i = 0; i < 6; i++) {
        for (int j = i + 1; j < 7; j++) {
            if (temp[i].rank < temp[j].rank)
                swapCards(temp[i], temp[j]);
        }
    }
    
    // �ˬd�P��
    int suitCount[4] = {0, 0, 0, 0};
    for (int i = 0; i < 7; i++) {
        suitCount[hand[i].suit]++;
    }
    bool flush = false;
    int flushSuit = -1;
    for (int s = 0; s < 4; s++) {
        if (suitCount[s] >= 5) {
            flush = true;
            flushSuit = s;
            break;
        }
    }
    // �ˬd���l�P��
    if (flush) {
        int flushRanks[7], flushCount = 0;
        for (int i = 0; i < 7; i++) {
            if (hand[i].suit == flushSuit)
                flushRanks[flushCount++] = hand[i].rank;
        }
        // �Ƨǡ]����^
        for (int i = 0; i < flushCount - 1; i++) {
            for (int j = i + 1; j < flushCount; j++) {
                if (flushRanks[i] < flushRanks[j]) {
                    int t = flushRanks[i];
                    flushRanks[i] = flushRanks[j];
                    flushRanks[j] = t;
                }
            }
        }
        int consecutive = 1;
        int bestSF = flushRanks[0];
        for (int i = 1; i < flushCount; i++) {
            if (flushRanks[i] == flushRanks[i - 1] - 1)
                consecutive++;
            else if (flushRanks[i] == flushRanks[i - 1])
                continue;
            else
                consecutive = 1;
            if (consecutive >= 5) {
                bestSF = flushRanks[i - 4];
                break;
            }
        }
        if (consecutive >= 5)
            return 8 * 1000000 + bestSF;
    }
    
    // �έp�U�P�I�W�v
    int freq[15] = {0};
    for (int i = 0; i < 7; i++) {
        int r = hand[i].rank + 2;
        freq[r]++;
    }
    
    // �K��P�_
    int four = -1;
    for (int r = 14; r >= 2; r--) {
        if (freq[r] == 4) { four = r; break; }
    }
    if (four != -1) {
        int kicker = 0;
        for (int r = 14; r >= 2; r--) {
            if (r != four && freq[r] > 0) { kicker = r; break; }
        }
        return 7 * 1000000 + four * 100 + kicker;
    }
    
    // ��Ī�P�_
    int three = -1, pair = -1;
    for (int r = 14; r >= 2; r--) {
        if (freq[r] >= 3 && three == -1)
            three = r;
        else if (freq[r] >= 2 && r != three && pair == -1)
            pair = r;
    }
    if (three != -1 && pair != -1)
        return 6 * 1000000 + three * 100 + pair;
    
    // �P��P�_ (�D���l�P��)
    if (flush) {
        int flushRanks[7], fCount = 0;
        for (int i = 0; i < 7; i++) {
            if (hand[i].suit == flushSuit)
                flushRanks[fCount++] = hand[i].rank;
        }
        for (int i = 0; i < fCount - 1; i++) {
            for (int j = i + 1; j < fCount; j++) {
                if (flushRanks[i] < flushRanks[j]) {
                    int t = flushRanks[i];
                    flushRanks[i] = flushRanks[j];
                    flushRanks[j] = t;
                }
            }
        }
        long long score = 5 * 1000000;
        long long mult = 10000;
        for (int i = 0; i < 5 && i < fCount; i++) {
            score += flushRanks[i] * mult;
            mult /= 10;
        }
        return score;
    }
    
    // ���l�P�_ (�D�P��)
    int unique[7], uCount = 0;
    for (int r = 14; r >= 2; r--) {
        if (freq[r] > 0)
            unique[uCount++] = r;
    }
    int straightHigh = -1;
    if (uCount >= 5) {
        int count = 1;
        for (int i = 1; i < uCount; i++) {
            if (unique[i] == unique[i - 1] - 1)
                count++;
            else
                count = 1;
            if (count >= 5) {
                straightHigh = unique[i - 4];
                break;
            }
        }
        // �S���p�GA-5-4-3-2
        if (count < 5 && unique[0] == 14) {
            bool has5 = false, has4 = false, has3 = false, has2 = false;
            for (int i = 0; i < uCount; i++){
                if(unique[i]==5) has5 = true;
                if(unique[i]==4) has4 = true;
                if(unique[i]==3) has3 = true;
                if(unique[i]==2) has2 = true;
            }
            if(has5 && has4 && has3 && has2)
                straightHigh = 5;
        }
    }
    if (straightHigh != -1)
        return 4 * 1000000 + straightHigh;
    
    // �T���P�_
    if (three != -1) {
        int kicker1 = 0, kicker2 = 0;
        for (int r = 14; r >= 2; r--) {
            if (r == three) continue;
            if (freq[r] > 0) {
                if (kicker1 == 0)
                    kicker1 = r;
                else if (kicker2 == 0) {
                    kicker2 = r;
                    break;
                }
            }
        }
        return 3 * 1000000 + three * 100 + kicker1 * 10 + kicker2;
    }
    
    // ���Τ@��P�_
    int firstPair = -1, secondPair = -1;
    for (int r = 14; r >= 2; r--) {
        if (freq[r] >= 2) {
            if (firstPair == -1)
                firstPair = r;
            else if (secondPair == -1) {
                secondPair = r;
                break;
            }
        }
    }
    if (firstPair != -1 && secondPair != -1) {
        int kicker = 0;
        for (int r = 14; r >= 2; r--) {
            if (r == firstPair || r == secondPair)
                continue;
            if (freq[r] > 0) { kicker = r; break; }
        }
        return 2 * 1000000 + firstPair * 100 + secondPair * 10 + kicker;
    }
    if (firstPair != -1) {
        int kickers[3] = {0}, idx = 0;
        for (int r = 14; r >= 2 && idx < 3; r--) {
            if (r == firstPair) continue;
            if (freq[r] > 0)
                kickers[idx++] = r;
        }
        return 1 * 1000000 + firstPair * 10000 + kickers[0] * 100 + kickers[1] * 10 + kickers[2];
    }
    
    // ���P�P�_
    int hc[5] = {0}, hcCount = 0;
    for (int r = 14; r >= 2 && hcCount < 5; r--) {
        if (freq[r] > 0)
            hc[hcCount++] = r;
    }
    return 0 * 1000000 + hc[0] * 10000 + hc[1] * 100 + hc[2] * 10 + hc[3];
}

// �P���r���ഫ�������ƭ�
int parseRank(const string &r) {
    if(r == "2") return 0;
    else if(r == "3") return 1;
    else if(r == "4") return 2;
    else if(r == "5") return 3;
    else if(r == "6") return 4;
    else if(r == "7") return 5;
    else if(r == "8") return 6;
    else if(r == "9") return 7;
    else if(r == "10") return 8;
    else if(r == "J" || r == "j") return 9;
    else if(r == "Q" || r == "q") return 10;
    else if(r == "K" || r == "k") return 11;
    else if(r == "A" || r == "a") return 12;
    else return -1;
}

// ���r���ഫ�������ƭȡA
int parseSuit(char s) {
    if(s=='c' || s=='C') return 0;
    else if(s=='d' || s=='D') return 1;
    else if(s=='h' || s=='H') return 2;
    else if(s=='s' || s=='S') return 3;
    else return -1;
}

// �a���~�ˬd
Card getValidCard(const string &owner) {
    Card c;
    string rankInput;
    char suitInput;
    bool valid = false;
    while (!valid) {
        cout << "�п�J" << owner << " (�榡: �P�� ���, �Ҧp A s): ";
        cin >> rankInput >> suitInput;
        int r = parseRank(rankInput);
        int s = parseSuit(suitInput);
        if (r == -1 || s == -1) {
            cout << "��J���~�I���Ī��P�����G2,3,4,5,6,7,8,9,10,J,Q,K,A�F���Ī�⦳�Gc, d, h, s�C�Э��s��J�C" << endl<< endl;
        } else {
            c.rank = r;
            c.suit = s;
            valid = true;
        }
    }
    return c;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    cout << "�w�{���J�u��D½�P�e�Ӳv�p�⾹�v" << endl<< endl;
    cout << "�i�P�X�����j" << endl;
    cout << "�P���G2,3,4,5,6,7,8,9,10,J,Q,K,A" << endl;
    cout << "���Gc (clubs), d (diamonds), h (hearts), s (spades)" << endl;
    cout << "�нT�O��J�榡���T�A�_�h�{���N�n�D�z���s��J�C" << endl;
	cout << "�Ƶ��G��J�榡�����^��j�p�g�C" << endl << endl;
    
    // ��J���a��P�G�Ĥ@�i
    Card player[2];
    player[0] = getValidCard("�z���Ĥ@�i�P");
    
    // �ĤG�i�P��J�ɻ��ˬd�O�_�P�Ĥ@�i����
    while (true) {
        Card temp = getValidCard("�z���ĤG�i�P");
        if (temp.rank == player[0].rank && temp.suit == player[0].suit) {
            cout << "���~�G�z���ĤG�i�P����P�Ĥ@�i�P�ۦP�I�Э��s��J�C" << endl;
        } else {
            player[1] = temp;
            break;
        }
    }
    
    // �������Ƴ]�w�]�i�̻ݨD�վ�^
    long long iterations ;
    cout << "�г]�w�������ơG";
	cin >> iterations;
    cout << endl;
    
    int wins = 0, ties = 0, losses = 0;
    
    // Monte Carlo �����G
    // �C���������A���إ� 52 �i�P���P�աA�A�������a��P���H���������P�P���@�P
    for (int i = 0; i < iterations; i++){
        Card deck[52];
        initDeck(deck);
        int deckSize = 52;
        
        // �q�P�դ��������a�Ĥ@�i�P
        for (int j = 0; j < deckSize; j++){
            if (deck[j].rank == player[0].rank && deck[j].suit == player[0].suit) {
                swapCards(deck[j], deck[deckSize - 1]);
                deckSize--;
                break;
            }
        }
        // �������a�ĤG�i�P
        for (int j = 0; j < deckSize; j++){
            if (deck[j].rank == player[1].rank && deck[j].suit == player[1].suit) {
                swapCards(deck[j], deck[deckSize - 1]);
                deckSize--;
                break;
            }
        }
        
        // �~�P�Ѿl�P
        shuffleDeck(deck, deckSize);
        
        // �H�������⪺��i��P�G���Ѿl�P�ժ��e��i
        Card opponent[2];
        opponent[0] = deck[0];
        opponent[1] = deck[1];
        
        // ���X 5 �i���@�P�]���U�Ӫ� 5 �i�P�^
        Card community[5];
        for (int j = 0; j < 5; j++){
            community[j] = deck[j + 2];
        }
        
        // �զ����a�P���U�� 7 �i�P�]��P + ���@�P�^
        Card playerHand[7], opponentHand[7];
        playerHand[0] = player[0];
        playerHand[1] = player[1];
        opponentHand[0] = opponent[0];
        opponentHand[1] = opponent[1];
        for (int j = 0; j < 5; j++){
            playerHand[j + 2] = community[j];
            opponentHand[j + 2] = community[j];
        }
        
        int playerScore = evaluateHand(playerHand);
        int opponentScore = evaluateHand(opponentHand);
        
        if (playerScore > opponentScore)
            wins++;
        else if (playerScore == opponentScore)
            ties++;
        else
            losses++;
    }
    
    double winRate = (wins + 0.5 * ties) / iterations * 100.0;
    cout << "\n���� " << iterations << " ����A�w���z����P�Ӳv���� " << winRate << " %\n";
    
    return 0;
}

