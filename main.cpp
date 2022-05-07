#include <iostream>
#include <queue>
#include <cmath>
#include <map>

#include "fairy_tail.hpp"

#include <cstdlib>
#include <ctime>

using namespace std;

const int INF = 100000;

const int N = 21;
const int SIZE = 10;

Direction directions[5] = { Direction::Left, Direction::Up, Direction::Right, Direction::Down, Direction::Pass };
const int changeX[4] = { -1, 0, 1, 0 };
const int changeY[4] = { 0, -1, 0, 1 };


struct answer {
    int res;
    pair<int, int> size;
    string map[SIZE][SIZE];
    
    answer(int _res, int(&ivanMap)[N][N], int(&elenaMap)[N][N],
        pair<int, int> delta,
        pair<int, int> minPos, pair<int, int> maxPos)
    {
        res = _res;
        size = { maxPos.second - minPos.second + 1, maxPos.first - minPos.first + 1};
        
        for (int i = 0; i < size.second; i++) {
            for (int j = 0; j < size.first; j++) {
                // составляем карту
                int num = max(ivanMap[i + minPos.first][j + minPos.second],
                    elenaMap[i + minPos.first - delta.first][j + minPos.second - delta.second]); // номер элемента
                switch (num)
                {
                case 1:
                    map[j][i] = ".";
                    break;
                case 2:
                    map[j][i] = ".";
                    break;
                case 3:
                    map[j][i] = "#";
                    break;
                default:
                    map[j][i] = "?";
                    break;
                }
            }
        }
        map[SIZE - minPos.second][SIZE - minPos.first] = "@";
        map[SIZE + delta.second - minPos.second][SIZE + delta.first - minPos.first] = "&";
    }
};


void scan(Fairyland& world, Character name, int x, int y, int (&map)[N][N]) {
    for (int i = 0; i < 4; i++)
    {
        if (map[x + changeX[i]][y + changeY[i]] == 0) {
            if (world.canGo(name, directions[i]))
                map[x + changeX[i]][y + changeY[i]] = 1;
            else
                map[x + changeX[i]][y + changeY[i]] = 3;
        }
    }
}

queue<int> checkTrack( int (&map)[N][N], int x, int y) {
    int track[N][N];
    bool use[N][N];    
    queue<pair<int, int>> queueTrack;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            use[i][j] = false;
            track[i][j] = -1;
        }
    }
    queueTrack.push({x, y});
    pair<int, int> res = { x, y };

    // bfs
    // нахождение ближайшей не пройденой клетки
    while (!queueTrack.empty())
    {
        pair<int, int> square = queueTrack.front();
        queueTrack.pop();

        if (use[square.first][square.second]) continue;
        else use[square.first][square.second] = true;

        if (map[square.first][square.second] == 1) {
            res = square;
            break;
        }

        for (int i = 0; i < 4; i++)
        {
            if (map[square.first + changeX[i]][square.second + changeY[i]] != 3 &&
                !use[square.first + changeX[i]][square.second + changeY[i]])
            {
                track[square.first + changeX[i]][square.second + changeY[i]] = i;
                queueTrack.push({square.first + changeX[i], square.second + changeY[i]});
            }
        }
    }

    // востановление пути
    vector<int> trackR;
    while (res.first != x || res.second != y)
    {
        int dn = track[res.first][res.second]; // direction number
        trackR.push_back(dn);
        res = { res.first - changeX[dn], res.second - changeY[dn] };
    }
    queue<int> out;
    for (int i = trackR.size() - 1; i >= 0; i--)
    {
        out.push(trackR[i]);
    }
    return out;
}


answer walk()
{
#pragma region start
    // данные Ивана
    int ivanMap[N][N];

    int ivanPresentPosX = SIZE;
    int ivanPresentPosY = SIZE;

    int ivanMinPosX = ivanPresentPosX;
    int ivanMinPosY = ivanPresentPosY;

    int ivanMaxPosX = ivanPresentPosX;
    int ivanMaxPosY = ivanPresentPosY;

    queue<int> ivanTrack;
    bool ivanExplored = false;

    // данные Елены
    int elenaMap[N][N];

    int elenaPresentPosX = SIZE;
    int elenaPresentPosY = SIZE;

    int elenaMinPosX = elenaPresentPosX;
    int elenaMinPosY = elenaPresentPosY;

    int elenaMaxPosX = elenaPresentPosX;
    int elenaMaxPosY = elenaPresentPosY;

    queue<int> elenaTrack;
    bool elenaExplored = false;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            ivanMap[i][j] = 0;
            elenaMap[i][j] = 0;
        }
    }
    ivanMap[ivanPresentPosX][ivanPresentPosY] = 2;
    elenaMap[elenaPresentPosX][elenaPresentPosY] = 2;
#pragma endregion
    Fairyland world;

    while (true)
    {
        // сканируем местность вокруг
        scan(world, Character::Ivan, ivanPresentPosX, ivanPresentPosY, ivanMap);
        scan(world, Character::Elena, elenaPresentPosX, elenaPresentPosY, elenaMap);

        // выбираем путь
        if (!ivanExplored && ivanTrack.empty())
        {
            ivanTrack = checkTrack(ivanMap, ivanPresentPosX, ivanPresentPosY);
            ivanExplored = ivanTrack.empty();
        }
        if (!elenaExplored && elenaTrack.empty())
        {
            elenaTrack = checkTrack(elenaMap, elenaPresentPosX, elenaPresentPosY);
            elenaExplored = elenaTrack.empty();
        }

        // если карта составлена, переходим к следующему этапу
        if (ivanExplored && elenaExplored) break;

        if (ivanExplored) ivanTrack.push(4);
        if (elenaExplored) elenaTrack.push(4);

        bool win = world.go(directions[ivanTrack.front()], directions[elenaTrack.front()]);

        // изменяем координаты
        ivanPresentPosX += changeX[ivanTrack.front()];
        ivanPresentPosY += changeY[ivanTrack.front()];

        elenaPresentPosX += changeX[elenaTrack.front()];
        elenaPresentPosY += changeY[elenaTrack.front()];

        // помечаем клетку как пройденую
        ivanMap[ivanPresentPosX][ivanPresentPosY] = 2;
        elenaMap[elenaPresentPosX][elenaPresentPosY] = 2;


        // собираем информацию

        ivanMinPosX = min(ivanMinPosX, ivanPresentPosX);
        ivanMinPosY = min(ivanMinPosY, ivanPresentPosY);
        ivanMaxPosX = max(ivanMaxPosX, ivanPresentPosX);
        ivanMaxPosY = max(ivanMaxPosY, ivanPresentPosY);

        elenaMinPosX = min(elenaMinPosX, elenaPresentPosX);
        elenaMinPosY = min(elenaMinPosY, elenaPresentPosY);
        elenaMaxPosX = max(elenaMaxPosX, elenaPresentPosX);
        elenaMaxPosY = max(elenaMaxPosY, elenaPresentPosY);

        // если Иван и Елена встретились
        if (win) {
            int deltaX = ivanPresentPosX - elenaPresentPosX;
            int deltaY = ivanPresentPosY - elenaPresentPosY;
            // проверярем погрешность
            if ((ivanTrack.front() + 2) % 4 == elenaTrack.front() && ivanTrack.front() != 4) {
                bool flag = max(ivanMaxPosX, elenaMaxPosX + deltaX) - min(ivanMaxPosX, elenaMaxPosX + deltaX) >= SIZE ||
                    max(ivanMaxPosY, elenaMaxPosY + deltaY) - min(ivanMaxPosY, elenaMaxPosY + deltaY) >= SIZE;

                for (int i = ivanMinPosX - 1; !flag && i <= ivanMaxPosX + 1; i++)
                    for (int j = ivanMinPosY - 1; j <= ivanMaxPosY + 1; j++)
                        if ((elenaMap[i - deltaX][j - deltaY] == 3 && (ivanMap[i][j] + 1) % 4 > 1) ||
                            (ivanMap[i][j] == 3 && (elenaMap[i - deltaX][j - deltaY] + 1) % 4 > 1))
                        {
                            // если не сходится
                            flag = true;
                            break;
                        }

                if (flag) switch (ivanTrack.front()) {
                case 0:
                    deltaX += 1;
                    break;
                case 1:
                    deltaY += 1;
                    break;
                case 2:
                    deltaX -= 1;
                    break;
                case 3:
                    deltaY -= 1;
                    break;
                }
            }
            return answer(world.getTurnCount(),
                ivanMap, elenaMap,
                { deltaX, deltaY },
                { min(ivanMinPosX, elenaMinPosX + deltaX), min(ivanMinPosY, elenaMinPosY + deltaY) },
                { max(ivanMaxPosX, elenaMaxPosX + deltaX), max(ivanMaxPosY, elenaMaxPosY + deltaY) });
        }

        // удаляем пройденый шаг из пути
        ivanTrack.pop();
        elenaTrack.pop();
    }
    // местность полностью изучена
    if (ivanMaxPosX - ivanMinPosX == elenaMaxPosX - elenaMinPosX &&
        ivanMaxPosY - ivanMinPosY == elenaMaxPosY - elenaMinPosY) {
        int deltaX = ivanMinPosX - elenaMinPosX;
        int deltaY = ivanMinPosX - elenaMinPosY;

        // проверка на совпадение
        bool flag = true;
        for (int i = ivanMinPosX; flag && i <= ivanMaxPosX; i++)
            for (int j = ivanMinPosY; j <= ivanMaxPosY; j++)
                if ((ivanMap[i][j] == 3 && (elenaMap[i - deltaX][j - deltaY] + 1) % 4 > 1) ||
                    (elenaMap[i - deltaX][j - deltaY] == 3 && (ivanMap[i][j] + 1) % 4 > 1))
                {
                    flag = false;
                    break;
                }
        if (!flag) {
            pair<int, int> dist[N][N];
            bool use[N][N];
            priority_queue<pair<int, pair<int, int>>> queueTrack;

            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                {
                    use[i][j] = false;
                    dist[i][j] = { INF, 4 };
                }
            queueTrack.push({ 0, {ivanPresentPosX, ivanPresentPosY} });
            dist[ivanPresentPosX][ivanPresentPosY] = { 0, 0 };

            // bfs
            // нахождение пути
            while (!queueTrack.empty())
            {
                pair<int, int> square = queueTrack.top().second;
                queueTrack.pop();

                if (use[square.first][square.second]) continue;
                else use[square.first][square.second] = true;

                for (int i = 0; i < 4; i++)
                {
                    int x = square.first + changeX[i];
                    int y = square.second + changeY[i];
                    if (ivanMap[x][y] != 3)
                    {
                        int d = dist[square.first][square.second].first + 1;
                        if (dist[x][y].first > d) {
                            dist[x][y] = { d, i };
                            queueTrack.push({ -d, {x, y} });
                        }
                    }
                }
            }

            // востановление пути
            pair<int, int> pos = { elenaPresentPosX + deltaX, elenaPresentPosY + deltaY };
            vector<int> track;
            while (pos.first != ivanPresentPosX || pos.second != ivanPresentPosY)
            {
                int dn = dist[pos.first][pos.second].second; // direction number
                track.push_back(dn);
                pos = { pos.first - changeX[dn], pos.second - changeY[dn] };
            }
            for (int i = 0; i < (track.size() + 1) / 2; i++)
            {
                if (world.go(
                    directions[track[track.size() - i - 1]],
                    directions[(track[i] + 2) % 4]))
                {
                    return answer(world.getTurnCount(),
                        ivanMap, elenaMap,
                        { deltaX, deltaY },
                        { min(ivanMinPosX, elenaMinPosX + deltaX), min(ivanMinPosY, elenaMinPosY + deltaY) },
                        { max(ivanMaxPosX, elenaMaxPosX + deltaX), max(ivanMaxPosY, elenaMaxPosY + deltaY) });
                }
            }
        }
    }

    // если они не могут встретится
    bool flag = false;
    for (int deltaX = -SIZE + 1; deltaX < SIZE; deltaX++){
        if (max(ivanMaxPosX, elenaMaxPosX + deltaX) - min(ivanMinPosX, elenaMinPosX + deltaX) >= SIZE) continue;
    
        for (int deltaY = -SIZE + 1; deltaY < SIZE; deltaY++) {
            if (max(ivanMaxPosY, elenaMaxPosY + deltaY) - min(ivanMinPosY, elenaMinPosY + deltaY) >= SIZE) continue;

            flag = true;
            for (int i = min(ivanMinPosX, elenaMinPosX + deltaX);
                flag && i <= max(ivanMaxPosX, elenaMaxPosX + deltaX); i++)
                for (int j = min(ivanMinPosY, elenaMinPosY + deltaY);
                    j <= max(ivanMaxPosY, elenaMaxPosY + deltaY); j++)
                    if (!((ivanMap[i][j] == 3 && elenaMap[i - deltaX][j - deltaY] == 3) || elenaMap[i - deltaX][j - deltaY] == 0 || ivanMap[i][j] == 0))
                    {
                        flag = false;
                        break;
                    }
            if (flag) {
                cout << deltaX << " " << deltaY;
                return answer(-1,
                    ivanMap, elenaMap,
                    { deltaX, deltaY },
                    { min(ivanMinPosX, elenaMinPosX + deltaX), min(ivanMinPosY, elenaMinPosY + deltaY) },
                    { max(ivanMaxPosX, elenaMaxPosX + deltaX), max(ivanMaxPosY, elenaMaxPosY + deltaY) });
            }
        }
    }
}


int main()
{
    cout << "start calculete" << endl;
    answer res = walk();
    cout << res.res << endl;
    for (int y = 0; y < res.size.first; y++)
    {
        for (int x = 0; x < res.size.second; x++)
        {
            cout << res.map[y][x];
        }
        cout << endl;
    }
}
