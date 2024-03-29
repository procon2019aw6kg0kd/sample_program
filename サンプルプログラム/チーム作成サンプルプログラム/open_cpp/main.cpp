#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <string>

// ファイル一行の最大文字数
#define MAX_LENGTH (256u)
// 1チームの最大コスト
#define MAX_COST (550u)
// 野手データ最大人数
#define MAX_BUTTER_NUM (500u)
// 投手データ最大人数
#define MAX_PITCHER_NUM (500u)
// 野手データファイル名
#define BUTTER_FILE_NAME ("Batter_Open.dat")
// 投手データファイル名
#define PITCHER_FILE_NAME ("Pitcher_Open.dat")

/*
 * 野手データ
 */
struct ButterData
{
    unsigned int id;             // ID
    std::string name;            // 氏名
    float daritsu;               // 打率
    float ichiruidaritsu;        // 一塁打率
    float niruidaritsu;          // 二塁打率
    float sanruidaritsu;         // 三塁打率
    float honruidaritsu;         // 本塁打率
    unsigned int cost;           // コスト

    bool operator<(const ButterData &another) const
    {
        return cost < another.cost;
    };
};

/*
 * 投手データ
 */
struct PitcherData
{
    unsigned int id;           // ID
    std::string name;          // 氏名
    float bougyoritsu;         // 防御率
    float yoshikyuuritsu;        // 与四球率
    unsigned int cost;         // コスト

    bool operator<(const ButterData &another) const
    {
        return cost < another.cost;
    };
};

class Main{
public :
	Main();
	virtual ~Main();

	bool readButterData();
	bool readPitcherData();
	bool selectPlayers();
	void displayTeam();
	void debugPrint();

private:
    bool isCostOver();
	// ファイルから読み込んだ野手データ
	std::vector<ButterData> m_butterData;
	// ファイルから読み込んだ投手データ
	std::vector<PitcherData> m_pitcherData;
	// 選択した野手データ
	std::vector<ButterData> m_selectedButterPlayers;
	// 選択した投手データ
	PitcherData m_seletedPitcherPlayer;

};

Main::Main()
: m_butterData()
, m_pitcherData()
, m_selectedButterPlayers()
, m_seletedPitcherPlayer()
{
}

Main::~Main(){}


/*
 * 選手を選ぶ(改造必須)
 */
bool
Main::selectPlayers()
{
    // 例として、コストが低い選手を野手9人、投手１人を選ぶ

    // 野手
    {
    	std::sort(m_butterData.begin(), m_butterData.end());
    	std::copy(m_butterData.begin(), m_butterData.begin()+9, std::back_inserter(m_selectedButterPlayers) );
    }

    // 投手
    {
        m_seletedPitcherPlayer.cost = UINT_MAX;
        for( unsigned int i = 0 ; i < m_pitcherData.size() ; i++)
        {
            if( m_seletedPitcherPlayer.cost > m_pitcherData[i].cost )
            {
                m_seletedPitcherPlayer = m_pitcherData[i];
            }
        }
    }

    if( isCostOver() )
    {
        fprintf(stderr, "cost over\n");
        return false;
    }

    return true;
}

/*
 * コストオーバーチェック
 */
bool
Main::isCostOver()
{
    unsigned int sum = 0u;

    unsigned int i;
    for( i = 0u ; i < 9 ; i++ )
    {
        sum += m_selectedButterPlayers[i].cost;
    }

    sum += m_seletedPitcherPlayer.cost;

    if( sum > MAX_COST )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * 処理結果出力
 */
void
Main::displayTeam()
{
    printf("Example Team\n");
    unsigned int i;
    for( i = 0u ; i < 9 ; i++ )
    {
        printf("%d#%s\n",
            m_selectedButterPlayers[i].id,
            m_selectedButterPlayers[i].name.c_str());
    }
    printf("%d#%s\n",
        m_seletedPitcherPlayer.id,
        m_seletedPitcherPlayer.name.c_str() );
}

/*
 * 文字列->UINT変換
 */
bool
analysisDataUInt(char* str, unsigned int& value)
{
    if( NULL == str )
    {
        fprintf(stderr, "analysis error %d\n", __LINE__);
        return false;
    }

    char *e;
    value = (unsigned int)strtoul(str, &e, 10);

    if (*e != '\0') {
		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	fprintf(stderr, "%d:%ld文字目の\'%c\',%dが変換不可\n", __LINE__, e-str+1, *e, *e);
    	return false;
    }

    return true;
}

/*
 * 文字列->Float変換
 */
int
analysisDataFloat(char* str, float& value)
{
    if( NULL == str )
    {
        fprintf(stderr, "analysis error %d\n", __LINE__);
        return false;
    }

    char *e;
    value = (float)strtod(str, &e);

    if (*e != '\0') {
		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	fprintf(stderr, "%d:%ld文字目の\'%c\',%dが変換不可\n", __LINE__, e-str+1, *e, *e);
    	return false;
    }

    return true;
}


/*
 * 野手データ読み込み
 */
bool
Main::readButterData()
{
    FILE* fp = fopen(BUTTER_FILE_NAME, "r");
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", BUTTER_FILE_NAME);
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];
    for( unsigned int i = 0u ; i < MAX_BUTTER_NUM ; i++)
    {
        // 一行読み込み
        memset(buffer, 0, sizeof(buffer));
        if (NULL == fgets(buffer, sizeof(buffer)-1, fp)) {
            break;
        }

        if( '\r' == buffer[strlen(buffer) - 2]  )
        {
            buffer[strlen(buffer) - 2] = '\0';
        }
        else if('\n' == buffer[strlen(buffer) - 1] )
        {
            buffer[strlen(buffer) - 1] = '\0';
        }

        ButterData data;
        // ID
        if(!analysisDataUInt(strtok(buffer, ","), data.id))
        {
            continue;
        }

        // 名前
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }
            data.name = tok;
        }

        // 打率
        if(!analysisDataFloat(strtok( NULL, ","), data.daritsu))
        {
            fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
            continue;
        }
        // 一塁打率
		if(!analysisDataFloat(strtok( NULL, ","), data.ichiruidaritsu))
		{
			fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
			continue;
		}
        // 二塁打率
		if(!analysisDataFloat(strtok( NULL, ","), data.niruidaritsu))
		{
			fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
			continue;
		}
        // 三塁打率
		if(!analysisDataFloat(strtok( NULL, ","), data.sanruidaritsu))
		{
			fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
			continue;
		}
        // 本塁打率
		if(!analysisDataFloat(strtok( NULL, ","), data.honruidaritsu))
		{
			fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
			continue;
		}
        // コスト
        if(!analysisDataUInt(strtok(NULL, ","), data.cost))
        {
            fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
            continue;
        }
        m_butterData.push_back(data);

    }

    fclose(fp);

    return true;
}

/*
 * 投手データ読み込み
 */
bool
Main::readPitcherData()
{
    FILE* fp = fopen(PITCHER_FILE_NAME, "r");
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", PITCHER_FILE_NAME);
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];

    unsigned int i;
    for( i = 0u ; i < MAX_PITCHER_NUM ; i++)
    {
        // 一行読み込み
        memset(buffer, 0, sizeof(buffer));
        if (NULL == fgets(buffer, sizeof(buffer)-1, fp)) {
            break;
        }

        if( '\r' == buffer[strlen(buffer) - 2]  )
        {
            buffer[strlen(buffer) - 2] = '\0';
        }
        else if('\n' == buffer[strlen(buffer) - 1] )
        {
            buffer[strlen(buffer) - 1] = '\0';
        }

        PitcherData data;
        // ID
        {
            char* tok = strtok(buffer, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
            data.id = (unsigned int)strtoul(tok, &e, 10);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

        // 名前
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }
            data.name = tok;
        }

        // 防御率
        if(!analysisDataFloat(strtok( NULL, ","), data.bougyoritsu))
        {
            fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
            continue;
        }
        // 与四球率
        if(!analysisDataFloat(strtok( NULL, ","), data.yoshikyuuritsu))
        {
            fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
            continue;
        }
        // コスト
        if(!analysisDataUInt(strtok(NULL, ","), data.cost))
        {
            fprintf(stderr, "%d行目解析エラー:%d\n", i+1, __LINE__);
            continue;
        }

        m_pitcherData.push_back(data);

    }

    fclose(fp);

    return true;
}


int main(void)
{
	Main obj;

    //printf("#野手データ読み込み\n");
    if(!obj.readButterData())
    {
        return 1;
    }

    // printf("#投手データ読み込み\n");
    if(!obj.readPitcherData())
    {
        return 1;
    }
    //obj.debugPrint();

    if(!obj.selectPlayers())
    {
        return 1;
    }

    obj.displayTeam();

    return 0;
}

void Main::debugPrint()
{
    printf("#######BUTTER\n");
    for( unsigned int i = 0u ; i < m_butterData.size() ; i++ )
    {
        printf("%d,%s,%f,%f,%f,%f,%f,%d\n",
            m_butterData[i].id,
            m_butterData[i].name.c_str(),
            m_butterData[i].daritsu,
            m_butterData[i].ichiruidaritsu,
            m_butterData[i].niruidaritsu,
            m_butterData[i].sanruidaritsu,
            m_butterData[i].honruidaritsu,
            m_butterData[i].cost);
    }

    printf("#######PITCHER\n");
    for( unsigned int i = 0u ; i < m_pitcherData.size() ; i++ )
    {
        printf("%d,%s,%f,%f,%d\n",
            m_pitcherData[i].id,
            m_pitcherData[i].name.c_str(),
            m_pitcherData[i].bougyoritsu,
            m_pitcherData[i].yoshikyuuritsu,
            m_pitcherData[i].cost);
    }
}
