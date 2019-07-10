#include <algorithm>
#include <array>
#include <cinttypes>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iterator>
#include <iomanip>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

constexpr const char* DEFAULT_INPUT[] =
{
	"BatFirstTeam.dat",
	"BatSecondTeam.dat",
	"Batter.dat",
	"Pitcher.dat",
};

// ファイル一行の最大文字数
constexpr size_t MAX_LENGTH = 512u;
// 1チームの最大コスト
constexpr unsigned int  MAX_COST = 550u;
// 野手データ最大人数
constexpr unsigned int MAX_BATTER_NUM = 65535u;
// 投手データ最大人数
constexpr unsigned int MAX_PITCHER_NUM = 65535u;
// 一試合のイニング数
constexpr int INING_NUM =9;
// 一試合の最大イニング数
constexpr int MAX_INING_NUM = 12;
// 延長イニング数
constexpr int ENTYOU_NUM = MAX_INING_NUM - INING_NUM;
// アウト数
constexpr int OUT_NUM = 3;

// fopenオプション
constexpr const char* FO_W = "w";
//constexpr const char* FO_W = "w,ccs=UTF-8";
constexpr const char* FO_R = "rb";
//constexpr const char* FO_R = "r,ccs=UTF-8";

enum class HIT_RESULT
{
	FIRST_BASE,
	SECOND_BASE,
	THIRD_BASE,
	HOMERUN,

	THREE_STRIKES,
	FOURE_BALL,
};

// ヒット結果から進塁数への変換
int HitResultToForwardBase(HIT_RESULT result)
{
	switch (result)
	{
	case HIT_RESULT::FIRST_BASE:
		return 1;
	case HIT_RESULT::SECOND_BASE:
		return 2;
	case HIT_RESULT::THIRD_BASE:
		return 3;
	case HIT_RESULT::HOMERUN:
		return 4;
	case HIT_RESULT::FOURE_BALL:
		return 1;	// 四球は1塁進む扱いにする
	default:
		break;
	}

	return 0;
}

// = BatterData

/*
 * 野手データ
 */
struct BatterData
{
    unsigned int	id;			// ID
    std::string		name;		// 氏名
    double			daritsu;	// 打率
	double			anda;		// 安打
	double			niruida;	// 二塁打(安打率＋二塁打率)
	double			sanruida;	// 三塁打(二塁打率＋三塁打率)
    unsigned int cost;			// コスト

	// ヒット結果判定
	HIT_RESULT ResultHit(void) const
	{
		const double rnd = static_cast<double>(rand()) / RAND_MAX;
		
		if( rnd < anda )
		{
			return HIT_RESULT::FIRST_BASE;
		}
		else if( rnd < niruida )
		{
			return HIT_RESULT::SECOND_BASE;
		}
		else if( rnd < sanruida )
		{
			return HIT_RESULT::THIRD_BASE;
		}
		else 
		{
			return HIT_RESULT::HOMERUN;
		}

		// no reach
	}
};

// = PitherData

/*
 * 投手データ
 */
struct PitcherData
{
    unsigned int	id;				// ID
    std::string		name;			// 氏名
    double			bougyoritsu;	// 防御率
    double			sikyuuritu;		// 与死球

    unsigned int cost;			// コスト
};

// = Team

class Team{
public :
	Team();
	virtual ~Team();

	bool Initialize(const std::string& team_data, const std::string& batter_data, const std::string& pitcher_data)
	{
		if (!readBatterData(batter_data))
		{
			return false;
		}

		if (!readPitcherData(pitcher_data))
		{
			return false;
		}

		if (!readPlayer(team_data))
		{
			return false;
		}

		return true;
	}


	void displayTeam();

	const std::string& Name() const
	{
		return m_teamName;
	}

	int Gettokuten(void)
	{
		return tokuten;
	}

	int Katen( int katen)
	{
		tokuten += katen;

		return tokuten;
	}
	
	const BatterData& Batter( void ) const
	{
		dajun = dajun % 9;
		return m_selectedBatterPlayers[dajun++];
	}
	
	const PitcherData& Pitcher( void ) const
	{
		return m_seletedPitcherPlayer;
	}

	int Cost() const
	{
		const unsigned int batter_cost = std::accumulate(
			m_selectedBatterPlayers.cbegin(), m_selectedBatterPlayers.cend(), 0u, [](unsigned int val, const BatterData& dat) -> unsigned int {
				return val += dat.cost;
			});

		return batter_cost + m_seletedPitcherPlayer.cost;
	}

private:

	bool readBatterData(const std::string& data_file);

	static std::pair< BatterData, bool > ConvertBatterDataOpenClass_(
		const std::string& data_file, unsigned int line_no, const std::vector< std::string >& fields);
	static std::pair< BatterData, bool > ConvertBatterDataBeginerClass_(
		const std::string& data_file, unsigned int line_no, const std::vector< std::string >& fields);


	bool readPitcherData(const std::string& data_file);

	static std::pair< PitcherData, bool > ConvertPitcherDataOpenClass_(
		const std::string& data_file, unsigned int line_no, const std::vector< std::string >& fields);
	static std::pair< PitcherData, bool > ConvertPitcherDataBeginerClass_(
		const std::string& data_file, unsigned int line_no, const std::vector< std::string >& fields);

	bool readPlayer(const std::string& player_data);

	// チーム名
	std::string m_teamName;
	// ファイルから読み込んだ野手データ
	std::map<unsigned int, BatterData> m_batterData;
	// ファイルから読み込んだ投手データ
	std::map<unsigned int, PitcherData> m_pitcherData;
	// 選択した野手データ
	std::vector<BatterData> m_selectedBatterPlayers;
	// 選択した投手データ
	PitcherData m_seletedPitcherPlayer;
	int tokuten;
	mutable int dajun;

};

Team::Team()
: m_batterData()
, m_pitcherData()
, m_selectedBatterPlayers()
, m_seletedPitcherPlayer()
{
	tokuten = 0;
	dajun = 0;
}

Team::~Team(){}

std::vector< std::string > SplitComma(const std::string& str)
{
	std::vector< std::string > result;

	std::stringstream ss(str);
	std::string buffer;
	while (std::getline(ss, buffer, ','))
	{
		result.push_back(buffer);
	}

	return result;
}

/*
 * 処理結果出力
 */
void
Team::displayTeam()
{
    printf(u8"<%s>\n", m_teamName.c_str());

	for (const auto& dat : m_selectedBatterPlayers)
	{
		printf(u8"%d#%s, %u\n", dat.id, dat.name.c_str(), dat.cost);
	}

	printf(u8"%d#%s, %u\n", m_seletedPitcherPlayer.id, m_seletedPitcherPlayer.name.c_str(), m_seletedPitcherPlayer.cost);

	printf(u8"TotalCost:%u\n", Cost());
}

/*
 * 野手データ読み込み
 */
bool
Team::readBatterData(const std::string& data_file)
{
    FILE* fp = fopen(data_file.c_str(), FO_R);
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, u8"fopen err %s\n", data_file.c_str());
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];
    for( unsigned int i = 0u ; i < MAX_BATTER_NUM ; i++)
    {
        // 一行読み込み
        memset(buffer, 0, sizeof(buffer));
        if (NULL == fgets(buffer, sizeof(buffer)-1, fp)) {
            break;
        }

		constexpr size_t OPEN_FIELDS_NUM = 8;
		constexpr size_t BEGINNER_FIELDS_NUM = 4;

		const auto fields = SplitComma(buffer);
		if ((fields.size() != OPEN_FIELDS_NUM) && (fields.size() != BEGINNER_FIELDS_NUM))
		{
			fprintf(stderr, u8"%s:%d missing fields\n", data_file.c_str(), i + 1);
			break;
		}

		const auto result = (fields.size() == OPEN_FIELDS_NUM)
			? (ConvertBatterDataOpenClass_(data_file, i, fields))
			: (ConvertBatterDataBeginerClass_(data_file, i, fields));

		if (!result.second)
		{
			continue;
		}

		m_batterData.insert(std::make_pair(result.first.id, result.first));
    }

    fclose(fp);

    return true;
}

// 野手データ読み込み(オープン)
std::pair< BatterData, bool > Team::ConvertBatterDataOpenClass_(const std::string& data_file, unsigned int line_no, const std::vector< std::string >& fields)
{
	std::pair< BatterData, bool > result = {{}, false};

	constexpr size_t INDEX_ID = 0;
	constexpr size_t INDEX_NAME = 1;
	constexpr size_t INDEX_AVERAGE_HIT = 2;
	constexpr size_t INDEX_AVERAGE_FIRST_BASE = 3;
	constexpr size_t INDEX_AVERAGE_SECOND_BASE = 4;
	constexpr size_t INDEX_AVERAGE_THIRD_BASE = 5;
	constexpr size_t INDEX_COST = 7;

	if (fields.size() < INDEX_COST)
	{
		fprintf(stderr, u8"%s:%d missing fields\n", data_file.c_str(), line_no + 1);
		return result;
	}

	BatterData& data = result.first;
	// ID
	try
	{
		data.id = std::stoul(fields[INDEX_ID]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d ID error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_ID].c_str(), e.what());

		return result;
	}

	// 名前
	try
	{
		data.name = fields[INDEX_NAME];
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d name error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_NAME].c_str(), e.what());

		return result;
	}

	// 打率
	try
	{
		data.daritsu = std::stod(fields[INDEX_AVERAGE_HIT]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d daritsu error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_AVERAGE_HIT].c_str(), e.what());

		return result;
	}

	// 一塁打率
	try
	{
		data.anda = std::stod(fields[INDEX_AVERAGE_FIRST_BASE]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d first base error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_AVERAGE_FIRST_BASE].c_str(), e.what());

		return result;
	}

	// 二塁打率
	try
	{
		data.niruida = data.anda + std::stod(fields[INDEX_AVERAGE_SECOND_BASE]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d second base error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_AVERAGE_SECOND_BASE].c_str(), e.what());

		return result;
	}

	// 三塁打率
	try
	{
		data.sanruida = data.niruida + std::stod(fields[INDEX_AVERAGE_THIRD_BASE]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d 3rd base error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_AVERAGE_THIRD_BASE].c_str(), e.what());

		return result;
	}

	// コスト
	try
	{
		data.cost = std::stoul(fields[INDEX_COST]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d cost error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_COST].c_str(), e.what());

		return result;
	}

	result.second = true;
	return result;
}

// 野手データ読み込み(ビギナー)
std::pair< BatterData, bool > Team::ConvertBatterDataBeginerClass_(const std::string& data_file, unsigned int line_no, const std::vector< std::string >& fields)
{
	std::pair< BatterData, bool > result = {{}, false};

	constexpr size_t INDEX_ID = 0;
	constexpr size_t INDEX_NAME = 1;
	constexpr size_t INDEX_AVERAGE_HIT = 2;
	constexpr size_t INDEX_COST = 3;

	if (fields.size() <= INDEX_COST)
	{
		fprintf(stderr, u8"%s:%d missing fields\n", data_file.c_str(), line_no + 1);
		return result;
	}

	BatterData& data = result.first;
	// ID
	try
	{
		data.id = std::stoul(fields[INDEX_ID]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d ID error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_ID].c_str(), e.what());

		return result;
	}

	// 名前
	try
	{
		data.name = fields[INDEX_NAME];
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d name error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_NAME].c_str(), e.what());

		return result;
	}

	// 打率
	try
	{
		data.daritsu = std::stod(fields[INDEX_AVERAGE_HIT]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d daritsu error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_AVERAGE_HIT].c_str(), e.what());

		return result;
	}

	// 塁打率(固定値)
	data.anda = 0.25;
	data.niruida = 0.25;
	data.sanruida = 0.25;

	// コスト
	try
	{
		data.cost = std::stoul(fields[INDEX_COST]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d cost error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_COST].c_str(), e.what());

		return result;
	}

	result.second = true;
	return result;
}

/*
 * 投手データ読み込み
 */
bool
Team::readPitcherData(const std::string& data_file)
{
    FILE* fp = fopen(data_file.c_str(), FO_R);
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, u8"fopen err %s\n", data_file.c_str());
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

		constexpr size_t OPEN_FIELDS_NUM = 5;
		constexpr size_t BEGINNER_FIELDS_NUM = 4;

		const auto fields = SplitComma(buffer);
		if ((fields.size() != OPEN_FIELDS_NUM) && (fields.size() != BEGINNER_FIELDS_NUM))
		{
			fprintf(stderr, u8"%s:%d missing fields %s\n", data_file.c_str(), i + 1, buffer);
			continue;
		}

		const auto result = (fields.size() == OPEN_FIELDS_NUM)
			? (ConvertPitcherDataOpenClass_(data_file, i, fields))
			: (ConvertPitcherDataBeginerClass_(data_file, i, fields));
		if (!result.second)
		{
			continue;
		}

        m_pitcherData.insert(std::make_pair(result.first.id, result.first));
    }

    fclose(fp);

    return true;
}

// 投手データ読み込み(オープン)
std::pair< PitcherData, bool > Team::ConvertPitcherDataOpenClass_(
	const std::string& data_file, unsigned int line_no, const std::vector< std::string >& fields)
{
	std::pair< PitcherData, bool > result = {{}, false};

	constexpr size_t INDEX_ID = 0;
	constexpr size_t INDEX_NAME = 1;
	constexpr size_t INDEX_AVERAGE_BOUGYO = 2;
	constexpr size_t INDEX_AVERAGE_SIKYU = 3;
	constexpr size_t INDEX_COST = 4;
	if (fields.size() <= INDEX_COST)
	{
		fprintf(stderr, u8"%s:%d missing fieldsn", data_file.c_str(), line_no + 1);
		return result;
	}

	PitcherData& data = result.first;
	// ID
	try
	{
		data.id = std::stoul(fields[INDEX_ID]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d ID error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_ID].c_str(), e.what());

		return result;
	}

	// 名前
	data.name = fields[INDEX_NAME];

	// 防御率
	try
	{
		const double bougyo = std::stod(fields[INDEX_AVERAGE_BOUGYO]);
		data.bougyoritsu = (0.0099 * bougyo * bougyo) - (0.039 * bougyo) + 0.2604;
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d BOUGYO error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_AVERAGE_BOUGYO].c_str(), e.what());

		return result;
	}

	// 四球率
	try
	{
		data.sikyuuritu = std::stod(fields[INDEX_AVERAGE_SIKYU]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d SIKJYUU error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_AVERAGE_SIKYU].c_str(), e.what());

		return result;
	}

	// コスト
	try
	{
		data.cost = std::stoul(fields[INDEX_COST]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d COST error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_COST].c_str(), e.what());

		return result;
	}

	result.second = true;

	return result;
}

// 投手データ読み込み(ビギナー)
std::pair< PitcherData, bool > Team::ConvertPitcherDataBeginerClass_(
	const std::string& data_file, unsigned int line_no, const std::vector< std::string >& fields)
{
	std::pair< PitcherData, bool > result = {{}, false};

	constexpr size_t INDEX_ID = 0;
	constexpr size_t INDEX_NAME = 1;
	constexpr size_t INDEX_AVERAGE_BOUGYO = 2;
	constexpr size_t INDEX_COST = 3;
	if (fields.size() <= INDEX_COST)
	{
		fprintf(stderr, u8"%s:%d missing fieldsn", data_file.c_str(), line_no + 1);
		return result;
	}

	PitcherData& data = result.first;
	// ID
	try
	{
		data.id = std::stoul(fields[INDEX_ID]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d ID error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_ID].c_str(), e.what());

		return result;
	}

	// 名前
	data.name = fields[INDEX_NAME];

	// 防御率
	try
	{
		const double bougyo = std::stod(fields[INDEX_AVERAGE_BOUGYO]);
		data.bougyoritsu = (0.0099 * bougyo * bougyo) - (0.039 * bougyo) + 0.2604;
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d BOUGYO error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_AVERAGE_BOUGYO].c_str(), e.what());

		return result;
	}

	// 四球率(固定値)
	data.sikyuuritu = 0.015;

	// コスト
	try
	{
		data.cost = std::stoul(fields[INDEX_COST]);
	}
	catch (std::exception& e)
	{
		fprintf(stderr, u8"%s:%d COST error->%s, %s\n", data_file.c_str(), line_no + 1, fields[INDEX_COST].c_str(), e.what());

		return result;
	}

	result.second = true;

	return result;
}

/*
 * 選択選手データ読み込み
 *
 * @attention 野手データ、投手データ読み込み後に実行する
 */
bool
Team::readPlayer(const std::string& player_data)
{
    FILE* fp = fopen(player_data.c_str(), FO_R);
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, u8"fopen err %s\n", player_data.c_str());
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];

	// 一行目はチーム名
	{
		if (NULL == fgets(buffer, sizeof(buffer) - 1, fp))
		{
			fprintf(stderr, u8"Team name gets err %s\n", player_data.c_str());

			return false;
		}

		// 改行を除いて取得する
		m_teamName = buffer;
		m_teamName = std::string(
			m_teamName.cbegin(), std::find_if(
				m_teamName.crbegin(), m_teamName.crend(), [](char c) -> bool {
					return (c != '\n') && (c != '\r');
				}).base());
	}

    unsigned int i;
    for( i = 0u ; i < 10 ; i++)
    {
        // 一行読み込み
        memset(buffer, 0, sizeof(buffer));
        if (NULL == fgets(buffer, sizeof(buffer)-1, fp)) {
            fprintf(stderr, u8"入力ファイルの行数(%d)が不足しています。入力ファイルはチーム名+バッター9+ピッチャー1の11行必要です\n", (i - 1) + 2 /* 現在読もうとした行を、タイトル行と0オリジンから1オリジンへの変換の+2*/);

			return false;
        }

		// 改行を除いて取得する
		const std::string tmp_buffer = buffer;
		const std::string line = std::string(
			tmp_buffer.cbegin(), std::find_if(
				tmp_buffer.crbegin(), tmp_buffer.crend(), [](char c) -> bool {
					return (c != '\n') && (c != '\r');
				}).base());

		unsigned int id = 0;
		try
		{
			id = std::stoul(line);
		}
		catch (std::exception& e)
		{
			fprintf(stderr, u8"%s:%d 数値変換エラー(%s):%s\n",
				player_data.c_str(), i + 2, e.what(), line.c_str());

			return false;
		}

		try
		{
			if( 9 == i )
			{
				// 投手
				m_seletedPitcherPlayer = m_pitcherData.at(id);
			}
			else
			{
				// 野手
				if (0 != std::count_if(
						m_selectedBatterPlayers.cbegin(), m_selectedBatterPlayers.cend(), [id](const BatterData& data){
							return (data.id == id);
						}))
				{
					fprintf(stderr, u8"%s:%d ID:%dが重複しています", player_data.c_str(), i + 2 /* チーム名の一行＋0オリジンから1オリジンへの変換 */, id);
					return false;
				}
				m_selectedBatterPlayers.push_back(m_batterData.at(id));
			}
		}
		catch (std::out_of_range& e)
		{
			fprintf(stderr, u8"%s:%d ID:%dは存在しません(%s)\n", player_data.c_str(), i + 2 /* チーム名の一行＋0オリジンから1オリジンへの変換 */, id, e.what());
			return false;
		}

    }

    fclose(fp);

    return true;
}

// = Base

class Base
{
public :
	Base()
		: m_Base()
	{
	}
	virtual ~Base()
	{
	}

	int Hit( HIT_RESULT hit );
	void Clear( void );

private:
	bool m_Base[4];		// 出塁者有無
};

// ヒット処理
int Base::Hit( HIT_RESULT hit )
{
	if (hit == HIT_RESULT::THREE_STRIKES)
	{
		return 0;
	}

	int tokuten = 0;
	
	m_Base[0] = true; // 打者BOXは常にtrue
	
	if (hit == HIT_RESULT::FOURE_BALL)
	{	// 四球処理 進塁先に選手がいる場合のみ、その選手を進塁させる
		auto p = std::find(std::begin(m_Base), std::end(m_Base), false);
		if (p == std::end(m_Base))
		{
			m_Base[0] = false;
			return 1;
		}

		*p = true;
		m_Base[0] = false;

		return 0;
	}

	// 出塁済みの選手をヒット結果数分進塁させる
	for( int i = 4 - 1 ; i >= 0 ; i-- )
	{
		// 出塁者あり？
		if( m_Base[i] )
		{
			// 出塁者をなしにする。
			m_Base[i] = false;
			
			const int j = i + HitResultToForwardBase(hit);

			// ホームベースに帰れば加点する。
			if( j > 4 - 1 )
			{
				tokuten++;
			}
			else
			{
				m_Base[j] = true; // 出塁した場所をture
			}
		}
	}
	
	return tokuten;
}

// クリア処理(出塁者を解除)
void Base::Clear( void )
{
	for( int i = 3 ; i > 0 ; i-- )
	{
		// 出塁者をなしにする。
		m_Base[i] = false;
	}
	
	m_Base[0] = true; // 打者BOXは常にtrue
}

// PlayData

class InningTeamPlayData
{
public:

	InningTeamPlayData() = default;
	InningTeamPlayData(const InningTeamPlayData&) = default;
	InningTeamPlayData(InningTeamPlayData&&) noexcept = default;
	InningTeamPlayData& operator=(const InningTeamPlayData&) = default;
	InningTeamPlayData& operator=(InningTeamPlayData&&) noexcept = default;
	~InningTeamPlayData() = default;

	void SetBatterResult(const BatterData& batter, HIT_RESULT result, int tokuten)
	{
		batter_result_.push_back(std::make_tuple(batter, result, tokuten));
	}

	size_t batterCount() const
	{
		return batter_result_.size();
	}

	// ヒット情報詳細
	void PrintHitVerbose(FILE* fp, const std::string& prefix) const
	{
		for (const auto& i : batter_result_)
		{
#if 0
			if (BatterHitResult(i) == 0)
			{
				continue;
			}
#endif

			// イニング
			fprintf(stdout, u8"%s", prefix.c_str());
			fprintf(fp, u8"%s", prefix.c_str());

			// 塁打/得点
			if (0 != BatterRunResult(i))
			{
				fprintf(stdout, u8"\t%s %2d点", BatterHitResultString(i).c_str(), BatterRunResult(i));
				fprintf(fp, u8"\t%s %2d点", BatterHitResultString(i).c_str(), BatterRunResult(i));
			}
			else
			{
				fprintf(stdout, u8"\t%s %*s", BatterHitResultString(i).c_str(), 4, u8"");
				fprintf(fp, u8"\t%s %*s", BatterHitResultString(i).c_str(), 4, u8"");
			}

			// 選手名(utf-8の文字幅対応が面倒なので末尾にもっていく)
			fprintf(stdout, u8" %s", std::get<0>(i).name.c_str());
			fprintf(fp, u8" %s", std::get<0>(i).name.c_str());

			fprintf(stdout, u8"\n");
			fprintf(fp, u8"\n");
		}

		if (!batter_result_.empty())
		{
			fprintf(stdout, u8"\n");
			fprintf(fp, u8"\n");
		}
	}

	void SetRun(int run)
	{
		run_ = run;
	}

	int Run() const
	{
		return run_;
	}

	void Validate()
	{
		valid_ = true;
	}

	bool IsValid() const
	{
		return valid_;
	}

private:

	inline static HIT_RESULT BatterHitResult(const std::tuple< BatterData, HIT_RESULT, int >& data)
	{
		return std::get<1>(data);
	}

	inline static std::string BatterHitResultString(const std::tuple< BatterData, HIT_RESULT, int >& data)
	{
		std::stringstream ss;
		const HIT_RESULT hit = BatterHitResult(data);
		switch (hit)
		{
		case HIT_RESULT::THREE_STRIKES:
			ss << u8"アウト";
			break;
		case HIT_RESULT::FIRST_BASE:
			ss << u8"一塁打";
			break;
		case HIT_RESULT::SECOND_BASE:
			ss << u8"二塁打";
			break;
		case HIT_RESULT::THIRD_BASE:
			ss << u8"三塁打";
			break;
		case HIT_RESULT::HOMERUN:
			ss << u8"本塁打";
			break;
		case HIT_RESULT::FOURE_BALL:
			ss << u8"　四球";
			break;
		default:
			// no reach
			break;
		}

		return ss.str();
	}

	inline static int BatterRunResult(const std::tuple< BatterData, HIT_RESULT, int >& data)
	{
		return std::get<2>(data);
	}

	std::vector< std::tuple< BatterData, HIT_RESULT, int > > batter_result_{};	// 打順ごとの結果(バッター情報, 塁打, 得点
	int run_{ 0 };	// 得点
	bool valid_{ false };

};

class TeamPlayData
{
public:

	TeamPlayData(const std::string& name) : team_name_(name){}
	TeamPlayData(const TeamPlayData&) = default;
	TeamPlayData(TeamPlayData&&) noexcept = default;
	TeamPlayData& operator=(const TeamPlayData&) = default;
	TeamPlayData& operator=(TeamPlayData&&) noexcept = default;
	~TeamPlayData() = default;

	const std::string& TeamName() const
	{
		return team_name_;
	}

	// イニングの結果設定
	void SetInningResult(int inning, const InningTeamPlayData& data)
	{
		inning_data_[inning] = data;
	}
	void SetInningResult(int inning, InningTeamPlayData&& data)
	{
		inning_data_[inning] = std::move(data);
	}

	// 得点取得
	int Runs() const
	{
		return std::accumulate(inning_data_.cbegin(), inning_data_.cend(), 0, [](int val, const InningTeamPlayData& data) -> int {
			return val + data.Run();
		});
	}

	// スコアボードプリント
	void PrintScoreBoard(FILE* fp) const
	{
		std::for_each(inning_data_.cbegin(), inning_data_.cend(), [fp](const InningTeamPlayData& data) -> void {
			if (data.IsValid())
			{
				fprintf(stdout, u8"%4d", data.Run());
				fprintf(fp, u8"%4d", data.Run());
			}
			else
			{
				fprintf(stdout, u8"%4s", u8"x");
				fprintf(fp, u8"%4s", u8"x");
			}
		});
	}

	// ヒット情報プリント(イニングごと)
	void PrintInningHitVerbose(FILE* fp, int inning, const std::string& prefix) const
	{
		const InningTeamPlayData& data = inning_data_[inning];
		if (!data.IsValid())
		{
			return;
		}

		data.PrintHitVerbose(fp, prefix);
	}

private:

	std::string team_name_;

	std::array< InningTeamPlayData, MAX_INING_NUM > inning_data_{};	// 各イニングごとの結果

	TeamPlayData() = delete;

};

class PlayData
{
public:

	PlayData(const std::string& bat_first_team, const std::string& bat_second_team)
		: bat_first_team_(bat_first_team)
		, bat_second_team_(bat_second_team)
	{
	}
	PlayData(const PlayData&) = default;
	PlayData& operator=(const PlayData&) = default;
	~PlayData() = default;

	const std::string& Name() const
	{
		return name_;
	}

	void SetInningResultBatFirstTeam(int inning, const InningTeamPlayData& data) { return bat_first_team_.SetInningResult(inning, data); };
	void SetInningResultBatFirstTeam(int inning, InningTeamPlayData&& data) { return bat_first_team_.SetInningResult(inning, std::move(data)); };
	void SetInningResultBatSecondTeam(int inning, const InningTeamPlayData& data) { return bat_second_team_.SetInningResult(inning, data); };
	void SetInningResultBatSecondTeam(int inning, InningTeamPlayData&& data) { return bat_second_team_.SetInningResult(inning, std::move(data)); };

	// 試合結果出力 UTF-8の文字幅判定が面倒なのでチーム名は改行して出力
	void PrintResult(FILE* fp) const
	{
		char buf[MAX_LENGTH] = {};
		bool bat_first_team_win = (bat_first_team_.Runs() > bat_second_team_.Runs());
		sprintf(buf, u8"<%s>\n%s\n", bat_first_team_.TeamName().c_str(), ResultString(bat_first_team_.Runs(), bat_second_team_.Runs()).c_str());
		fprintf(stdout, u8"%s", buf); fprintf(fp, u8"%s", buf);
		sprintf(buf, u8"<%s>\n%s\n", bat_second_team_.TeamName().c_str(), ResultString(bat_second_team_.Runs(), bat_first_team_.Runs()).c_str());
		fprintf(stdout, u8"%s", buf); fprintf(fp, u8"%s", buf);
		sprintf(buf, u8"<%s> %d - %d <%s>\n", bat_first_team_.TeamName().c_str(), bat_first_team_.Runs(), bat_second_team_.Runs(), bat_second_team_.TeamName().c_str());
		fprintf(stdout, u8"%s", buf); fprintf(fp, u8"%s", buf);
	}

	// 試合経過出力 UTF-8の文字幅判定が面倒なのでチーム名は改行して出力
	void PrintGameProgress(FILE* fp, bool verbose) const
	{
		// スコアボード表示
		char buf[MAX_LENGTH] = {};
		sprintf(buf, u8"<%s>\n", bat_first_team_.TeamName().c_str());
		fprintf(stdout, u8"%s", buf); fprintf(fp, u8"%s", buf);
		bat_first_team_.PrintScoreBoard(fp);
		fprintf(stdout, u8"\n"); fprintf(fp, u8"\n");

		sprintf(buf, u8"<%s>\n", bat_second_team_.TeamName().c_str());
		fprintf(stdout, u8"%s", buf); fprintf(fp, u8"%s", buf);
		bat_second_team_.PrintScoreBoard(fp);
		fprintf(stdout, u8"\n"); fprintf(fp, u8"\n");

		if (!verbose)
		{
			return;
		}

		// イニングごとのヒット
		fprintf(stdout, u8"\n詳細\n"); fprintf(fp, u8"\n詳細\n");

		for (int i = 0; i < MAX_INING_NUM; ++i)
		{
			std::stringstream ss;
			ss << std::setw(2) << std::setfill('0') << (i + 1) << u8"回";
			bat_first_team_.PrintInningHitVerbose(fp, i, ss.str() + u8"表");
			bat_second_team_.PrintInningHitVerbose(fp, i, ss.str() + u8"裏");
		}

	}

private:

	static std::string ResultString(int mine_runs, int opposition_runs)
	{
		if (mine_runs == opposition_runs)
		{
			return u8"even";
		}
		return (mine_runs > opposition_runs) ? (u8"win") : (u8"lose");
	}

private:

	std::string name_;

	TeamPlayData bat_first_team_;
	TeamPlayData bat_second_team_;

	PlayData() = delete;

};

//

namespace Util
{

template< class C >
constexpr auto Size(const C& v) -> decltype(v.size()) { return v.size(); }

	template< typename T, size_t SZ >
constexpr size_t Size(const T (&)[SZ]) noexcept { return SZ; }

}

// コマンドオプションを扱いやすいようvectorに変更
std::vector< std::string > OptStore(int argc, char** argv)
{
	std::vector< std::string > result;

	if (argc == 5)
	{
		result.insert(result.end(), argv, argv + argc);

		return result;
	}

	if (argc == 1)
	{
		result.push_back(argv[0]);
		result.insert(result.end(), DEFAULT_INPUT, DEFAULT_INPUT + Util::Size(DEFAULT_INPUT));

		return  result;
	}

	fprintf(stderr, u8"usage %s [先攻チーム選手データ 後攻チーム選手データ 野手データ 投手データ]\n", argv[0]);
	fprintf(stderr, u8"省略時 先攻チーム選手データ:%s\n", DEFAULT_INPUT[0]);
	fprintf(stderr, u8"       後攻チーム選手データ:%s\n", DEFAULT_INPUT[1]);
	fprintf(stderr, u8"       野手データ:%s\n", DEFAULT_INPUT[2]);
	fprintf(stderr, u8"       投手データ:%s\n", DEFAULT_INPUT[3]);

	return result;
}

void PlayBall( Team &senkou, Team &koukou );

int main( int argc , char** argv )
{
	setlocale(LC_ALL, "ja_JP.utf-8");

	// コマンドオプション
	const std::vector< std::string > options(OptStore(argc, argv));
	if (options.empty())
	{
		return -1;
	}

	const std::string& bat_first_team = options[1];
	const std::string& bat_second_team = options[2];
	const std::string& batter_data = options[3];
	const std::string& pitcher_data = options[4];

	Team senkou;	// 先攻チームデータ
	if (!senkou.Initialize(bat_first_team, batter_data, pitcher_data))
	{
		return 1;
	}

	Team koukou;	// 後攻チームデータ
	if (!koukou.Initialize(bat_second_team, batter_data, pitcher_data))
	{
		return 1;
	}

	senkou.displayTeam();
	koukou.displayTeam();

	const bool senkou_cost_over = (senkou.Cost() > MAX_COST);
	const bool koukou_cost_over = (koukou.Cost() > MAX_COST);
	if (senkou_cost_over || koukou_cost_over)
	{
		FILE* fp = fopen("result.txt", FO_W);

		char buf[MAX_LENGTH] = {};
		sprintf(buf, u8"<%s>\n%s\n", senkou.Name().c_str(), (senkou_cost_over) ? (u8"cost over") : (u8"win"));
		fprintf(stdout, u8"%s", buf); fprintf(fp, "%s", buf);
		sprintf(buf, u8"<%s>\n%s\n", koukou.Name().c_str(), (koukou_cost_over) ? (u8"cost over") : (u8"win"));
		fprintf(stdout, u8"%s", buf); fprintf(fp, u8"%s", buf);

		fclose(fp);

		return 0;
	}

	srand(static_cast<unsigned int>(time(NULL)));

	// 試合
	PlayBall(senkou,koukou);

	return 0;
}

InningTeamPlayData Play( Team &seme, Team &mamori );

HIT_RESULT Chkhit( const BatterData& batter , const PitcherData& pitcher );
std::pair< bool, bool > IsHit( const BatterData& batter , const PitcherData& pitcher );	// 戻り値first ヒット有無, second 四球有無

bool IsInningFinishBatFirst(int inning, int first_run, int second_run)
{
	if (inning < (INING_NUM - 1))
	{	// 8回までは無条件で終了しない
		return false;
	}

	// 9回以降は後攻側が負けていれば終了しない
	if (first_run >= second_run)
	{
		return false;
	}

	// 9以降かつ先攻側が勝っていれば試合終了
	return true;
}

bool IsInningFinishBatSecond(int inning, int first_run, int second_run)
{
	if (inning < (INING_NUM - 1))
	{	// 8回までは無条件で終了しない
		return false;
	}

	// 9回以降はどちらかのチームが勝っていれば終了する
	if (first_run != second_run)
	{
		return true;
	}

	if (inning >= (MAX_INING_NUM - 1))
	{	// 最大イニングまで行けば終了する
		return true;
	}

	// 9回以降かつ最大イニング未満で同一得点なら試合続行
	return false;
}

void PlayBall( Team &senkou, Team &koukou )
{
	PlayData play_data(senkou.Name(), koukou.Name());

	for (int inning = 0; inning < MAX_INING_NUM; ++inning)
	{
		// 先攻の攻撃
		play_data.SetInningResultBatFirstTeam(inning, Play( senkou , koukou ));
		if (IsInningFinishBatFirst(inning, senkou.Gettokuten(), koukou.Gettokuten()))
		{
			break;
		}

		// 後攻の攻撃
		play_data.SetInningResultBatSecondTeam(inning, Play( koukou , senkou ));
		if (IsInningFinishBatSecond(inning, senkou.Gettokuten(), koukou.Gettokuten()))
		{
			break;
		}
	}

	{
		FILE* fp = fopen("result.txt", FO_W);
		play_data.PrintResult(fp);
		play_data.PrintGameProgress(fp, true);
		fclose(fp);
	}
}

// 1イニング
InningTeamPlayData  Play( Team &seme, Team &mamori)
{
	InningTeamPlayData inning_team_result;

	int out = 0;		// アウト数
	int tokuten = 0;	// 得点

	Base base;	// 進塁情報
	
	// アウトが規定値に達すれば処理を終了
	while( OUT_NUM > out )
	{
		// ヒット結果を判定
		const BatterData batter = seme.Batter();
		const HIT_RESULT hit = Chkhit( batter, mamori.Pitcher() );

		// 出塁数が0であればアウトとする
		const int before_tokuten = tokuten;
		if( HIT_RESULT::THREE_STRIKES == hit )
		{
			out++;
		}
		else
		{
			// ヒットであれば出塁処理と得点の加算を行う。
			tokuten = tokuten + base.Hit(hit);
		}

		inning_team_result.SetBatterResult(batter, hit, tokuten - before_tokuten);
	}

	// 得点を攻撃チームに加える。
	seme.Katen(tokuten);
	inning_team_result.SetRun(tokuten);

	inning_team_result.Validate();
	return inning_team_result;
}

// ヒット確認
HIT_RESULT Chkhit( const BatterData& batter , const PitcherData& pitcher )
{
	// ヒット判定
	auto hit_result = IsHit(batter, pitcher);
	if (hit_result.first)
	{	// ヒットであればヒット種別を算出する。
		return batter.ResultHit();
	}

	if (hit_result.second)
	{	// 四球
		return HIT_RESULT::FOURE_BALL;
	}

	// 三振
	return HIT_RESULT::THREE_STRIKES;
}

// ヒット判定
// 戻り値first ヒット有無, second 四球有無
std::pair< bool, bool > IsHit( const BatterData& batter , const PitcherData& pitcher )
{
	const double bougyoritu = pitcher.bougyoritsu;
	const double daritu = batter.daritsu;
	
	// ヒット率を取得
	const double hitritu = ( bougyoritu + daritu ) / 2.0;
	
	// 四球率を取得
	const double sikyuuritu = pitcher.sikyuuritu;
	
	const double rnd1 = static_cast<double>(rand()) / RAND_MAX;
	
	if (rnd1 < hitritu)
	{
		return std::make_pair(true, false);
	}
	if (rnd1 < (hitritu + sikyuuritu))
	{
		return std::make_pair(false, true);
	}

	return std::make_pair(false, false);
}

