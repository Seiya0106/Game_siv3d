#include <Siv3D.hpp>

// シーン間で共有するデータ
struct GameData
{
	bool unlockedStage1 = false;
	bool unlockedStage2 = false;
	bool unlockedStage3 = false;
};

// シーンのキー
enum class State
{
	Title,
	Credit,
	Stage1,
	Stage2,
	Stage3,
};

// GameManager の型エイリアス
using App = SceneManager<State, GameData>;

// カスタムボタン関数
bool Button(const Rect& rect, const Font& font, const String& text, bool enabled)
{
	// マウスカーソルがボタンの上にある場合
	if (enabled && rect.mouseOver())
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	const RoundRect roundRect = rect.rounded(6);

	// 影と背景を描く
	roundRect
		.drawShadow(Vec2{ 2, 2 }, 12, 0)
		.draw(ColorF{ 1.0, 0.94, 0.60 });

	//　枠を描く
	rect.stretched(-3).rounded(3)
		.drawFrame(2, ColorF{ 0.4, 0.3, 0.2 });

	// テキストを描く
	font(text).drawAt(40, rect.center(), ColorF{ 0.4, 0.3, 0.2 });

	// 無効の場合
	if (!enabled)
	{
		// グレーの半透明を重ねる
		roundRect.draw(ColorF{ 0.8, 0.8 });
		// ボタンが押せなくなる
		return false;
	}

	// ボタンを左クリックするとtrueを返す
	return rect.leftClicked();
}

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init)
		: IScene{ init }
	{
		// 背景の色を設定する
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });
	}

	void update() override
	{
		// Credit
		if (Button(Rect{ 10, 10, 150, 80 }, m_font, U"Credit", true))
		{
			// Creditシーンに移動
			changeScene(State::Credit);
		}

		// Tutorial
		if (Button(Rect{ 270, 270, 250, 70 }, m_font, U"Tutorial", true))
		{
			// Stage1 をアンロック
			getData().unlockedStage1 = true;
		}

		// Stage1
		if (Button(Rect{ 80, 400, 200, 80 }, m_font, U"Stage1", getData().unlockedStage1))
		{
			// Stage1 シーンに移動
			changeScene(State::Stage1);
		}

		// Stage2
		if (Button(Rect{ 300, 400, 200, 80 }, m_font, U"Stage2", getData().unlockedStage2))
		{
			// Stage2 シーンに移動
			changeScene(State::Stage2);
		}

		// Stage3
		if (Button(Rect{ 520, 400, 200, 80 }, m_font, U"Stage3", getData().unlockedStage3))
		{
			// Stage3 シーンに移動
			changeScene(State::Stage3);
		}
	}

	void draw() const override
	{
		// タイトル
		m_font(U"Title").draw(80, Vec2{ 300, 150 }, ColorF{ 0.2 });

		// ボタンの描画は update() 内で完結しているため、ここでは何もしない
	}

private:

	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
};

class Credit : public App::Scene
{
public:
	
	Credit(const InitData& init)
	: IScene{ init }
	{
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });
	}
	
	void update() override
	{
		// 戻るボタン
		if (Button(Rect{ 10, 10, 200, 70 }, m_font, U"BackMenu", true))
		{
			// タイトルシーンに戻る
			changeScene(State::Title);
		}
		m_font(U"プランナー").draw(32, Vec2{ 80, 100 }, ColorF{ 0.0 });
		m_font(U"Seiya").draw(32, Vec2{100, 140}, ColorF{ 0.0 });
		m_font(U"プログラマー").draw(32, Vec2{ 80, 200 }, ColorF{ 0.0 });
		m_font(U"Seiya").draw(32, Vec2{ 100, 240}, ColorF{ 0.0 });
		m_font(U"bukinyan").draw(32, Vec2{ 200, 240 }, ColorF{ 0.0 });
		m_font(U"kanaka").draw(32, Vec2{ 350, 240 }, ColorF{ 0.0 });
		m_font(U"使用素材").draw(32, Vec2{ 80, 300 }, ColorF{ 0.0 });
		m_font(U"illustAC: https://www.ac-illust.com/").draw(32, Vec2{ 100, 340 }, ColorF{ 0.0 });
	}
private:
	const Font m_font{ FontMethod::MSDF, 32 };
};

// ステージ1
class Stage1 : public App::Scene
{
public:

	Stage1(const InitData& init)
		: IScene{ init }
	    , needle(U"../App/example/needle.png")
	{
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });
	}

	void update() override
	{
		// 戻るボタン
		//　現在は戻るだけで次のボタンが押せるようになっている
		if (Button(Rect{ 10, 10, 200, 70 }, m_font, U"BackMenu", true))
		{
			// Stage2 をアンロック
			getData().unlockedStage2 = true;
			// タイトルシーンに戻る
			changeScene(State::Title);
		}
		// リスタートボタン
		if (Button(Rect{ 10, 90, 200, 70}, m_font, U"ReStart", true))
		{
			// 処理内容
			Print << U"Pushed";
		}
		// 設置物をおくところの背景
		Rect{ 40, 170, 130, 130}.draw();
		Rect{ 40, 310, 130, 130}.draw();
		Rect{ 40, 450, 130, 130}.draw();
		
		// 境界線ようの縦線
		Rect{ 230, 0, 10, 600}.draw(ColorF{ 0 });
		
		// 針を描画
		needle.resized(200,200).drawAt(Scene::Center());
	}

private:

	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Texture needle;
};

// ステージ2
class Stage2 : public App::Scene
{
public:

	Stage2(const InitData& init)
		: IScene{ init }
	{
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });
	}

	void update() override
	{
		// 戻るボタン
		//　現在は戻るだけで次のボタンが押せるようになっている
		if (Button(Rect{ 10, 10, 200, 70 }, m_font, U"BackMenu", true))
		{
			// Stage2 をアンロック
			getData().unlockedStage3 = true;
			// タイトルシーンに戻る
			changeScene(State::Title);
		}
		// リスタートボタン
		if (Button(Rect{ 10, 90, 200, 70}, m_font, U"ReStart", true))
		{
			// 処理内容
			Print << U"Pushed";
		}
		// 設置物をおくところの背景
		Rect{ 40, 170, 130, 130}.draw();
		Rect{ 40, 310, 130, 130}.draw();
		Rect{ 40, 450, 130, 130}.draw();
		
		// 境界線ようの縦線
		Rect{ 230, 0, 10, 600}.draw(ColorF{ 0 });
	}

private:

	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
};

//　ステージ3
class Stage3 : public App::Scene
{
public:

	Stage3(const InitData& init)
		: IScene{ init }
	{
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });
	}

	void update() override
	{
		// 戻るボタン
		if (Button(Rect{ 10, 10, 200, 70 }, m_font, U"BackMenu", true))
		{
			// タイトルシーンに戻る
			changeScene(State::Title);
		}
		// リスタートボタン
		if (Button(Rect{ 10, 90, 200, 70}, m_font, U"ReStart", true))
		{
			// 処理内容
			Print << U"Pushed";
		}
		// 設置物をおくところの背景
		Rect{ 40, 170, 130, 130}.draw();
		Rect{ 40, 310, 130, 130}.draw();
		Rect{ 40, 450, 130, 130}.draw();
		
		// 境界線ようの縦線
		Rect{ 230, 0, 10, 600}.draw(ColorF{ 0 });
	}

private:

	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
};


void Main()
{
	// シーンマネージャーを作成
	App manager;

	// 各シーンを登録
	manager.add<Title>(State::Title);
	manager.add<Credit>(State::Credit);
	manager.add<Stage1>(State::Stage1);
	manager.add<Stage2>(State::Stage2);
	manager.add<Stage3>(State::Stage3);

	// タイトルシーンから開始
	manager.init(State::Title);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
