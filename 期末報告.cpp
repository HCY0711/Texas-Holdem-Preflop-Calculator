#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// 卡牌結構：rank 0~12 代表 2 ~ Ace；
//suit 0～3 分別代表 clubs, diamonds, hearts, spades
struct Card {
    int rank;  // 0 -> 2, ... , 12 -> Ace
    int suit;  // 0: clubs, 1: diamonds, 2: hearts, 3: spades
};

// 交換兩張牌
void swapCards(Card &a, Card &b) {
    Card temp = a;
    a = b;
    b = temp;
}

// 初始化 52 張牌
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

// Fisher–Yates 洗牌
void shuffleDeck(Card deck[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swapCards(deck[i], deck[j]);
    }
}

// 簡化版 7 張牌手牌評估函式：分數越高表示牌型越強。
// 分類依序為：
// 8: 順子同花, 7: 鐵支, 6: 葫蘆, 5: 同花, 4: 順子,
// 3: 三條, 2: 兩對, 1: 一對, 0: 高牌
int evaluateHand(Card hand[7]) {
    // 複製手牌並依 rank 降冪排序
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
    
    // 檢查同花
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
    // 檢查順子同花
    if (flush) {
        int flushRanks[7], flushCount = 0;
        for (int i = 0; i < 7; i++) {
            if (hand[i].suit == flushSuit)
                flushRanks[flushCount++] = hand[i].rank;
        }
        // 排序（遞減）
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
    
    // 統計各牌點頻率
    int freq[15] = {0};
    for (int i = 0; i < 7; i++) {
        int r = hand[i].rank + 2;
        freq[r]++;
    }
    
    // 鐵支判斷
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
    
    // 葫蘆判斷
    int three = -1, pair = -1;
    for (int r = 14; r >= 2; r--) {
        if (freq[r] >= 3 && three == -1)
            three = r;
        else if (freq[r] >= 2 && r != three && pair == -1)
            pair = r;
    }
    if (three != -1 && pair != -1)
        return 6 * 1000000 + three * 100 + pair;
    
    // 同花判斷 (非順子同花)
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
    
    // 順子判斷 (非同花)
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
        // 特殊狀況：A-5-4-3-2
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
    
    // 三條判斷
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
    
    // 兩對或一對判斷
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
    
    // 高牌判斷
    int hc[5] = {0}, hcCount = 0;
    for (int r = 14; r >= 2 && hcCount < 5; r--) {
        if (freq[r] > 0)
            hc[hcCount++] = r;
    }
    return 0 * 1000000 + hc[0] * 10000 + hc[1] * 100 + hc[2] * 10 + hc[3];
}

// 牌號字串轉換成內部數值
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

// 花色字元轉換成內部數值，
int parseSuit(char s) {
    if(s=='c' || s=='C') return 0;
    else if(s=='d' || s=='D') return 1;
    else if(s=='h' || s=='H') return 2;
    else if(s=='s' || s=='S') return 3;
    else return -1;
}

// 帶錯誤檢查
Card getValidCard(const string &owner) {
    Card c;
    string rankInput;
    char suitInput;
    bool valid = false;
    while (!valid) {
        cout << "請輸入" << owner << " (格式: 牌號 花色, 例如 A s): ";
        cin >> rankInput >> suitInput;
        int r = parseRank(rankInput);
        int s = parseSuit(suitInput);
        if (r == -1 || s == -1) {
            cout << "輸入錯誤！有效的牌號有：2,3,4,5,6,7,8,9,10,J,Q,K,A；有效花色有：c, d, h, s。請重新輸入。" << endl<< endl;
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
    
    cout << "德州撲克「單挑翻牌前勝率計算器」" << endl<< endl;
    cout << "【牌碼說明】" << endl;
    cout << "牌號：2,3,4,5,6,7,8,9,10,J,Q,K,A" << endl;
    cout << "花色：c (clubs), d (diamonds), h (hearts), s (spades)" << endl;
    cout << "請確保輸入格式正確，否則程式將要求您重新輸入。" << endl;
	cout << "備註：輸入格式不分英文大小寫。" << endl << endl;
    
    // 輸入玩家手牌：第一張
    Card player[2];
    player[0] = getValidCard("您的第一張牌");
    
    // 第二張牌輸入時需檢查是否與第一張重複
    while (true) {
        Card temp = getValidCard("您的第二張牌");
        if (temp.rank == player[0].rank && temp.suit == player[0].suit) {
            cout << "錯誤：您的第二張牌不能與第一張牌相同！請重新輸入。" << endl;
        } else {
            player[1] = temp;
            break;
        }
    }
    
    // 模擬次數設定（可依需求調整）
    long long iterations ;
    cout << "請設定模擬次數：";
	cin >> iterations;
    cout << endl;
    
    int wins = 0, ties = 0, losses = 0;
    
    // Monte Carlo 模擬：
    // 每次模擬中，先建立 52 張牌的牌組，再移除玩家手牌後隨機抽取對手手牌與公共牌
    for (int i = 0; i < iterations; i++){
        Card deck[52];
        initDeck(deck);
        int deckSize = 52;
        
        // 從牌組中移除玩家第一張牌
        for (int j = 0; j < deckSize; j++){
            if (deck[j].rank == player[0].rank && deck[j].suit == player[0].suit) {
                swapCards(deck[j], deck[deckSize - 1]);
                deckSize--;
                break;
            }
        }
        // 移除玩家第二張牌
        for (int j = 0; j < deckSize; j++){
            if (deck[j].rank == player[1].rank && deck[j].suit == player[1].suit) {
                swapCards(deck[j], deck[deckSize - 1]);
                deckSize--;
                break;
            }
        }
        
        // 洗牌剩餘牌
        shuffleDeck(deck, deckSize);
        
        // 隨機抽取對手的兩張手牌：取剩餘牌組的前兩張
        Card opponent[2];
        opponent[0] = deck[0];
        opponent[1] = deck[1];
        
        // 取出 5 張公共牌（接下來的 5 張牌）
        Card community[5];
        for (int j = 0; j < 5; j++){
            community[j] = deck[j + 2];
        }
        
        // 組成玩家與對手各自 7 張牌（手牌 + 公共牌）
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
    cout << "\n模擬 " << iterations << " 次後，預估您的手牌勝率約為 " << winRate << " %\n";
    
    return 0;
}

