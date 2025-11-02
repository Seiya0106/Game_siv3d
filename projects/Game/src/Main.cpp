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
	Tutorial,
	Stage1,
	Stage2,
	Stage3,
	GameOver,
	Clear,
};

// 抽象的なインターフェース（ドラッグ可能なオブジェクトの共通機能）
struct IDraggable
{
	virtual void update() = 0;
	virtual void draw() const = 0;
	virtual void reset() = 0;
	virtual ~IDraggable() = default;
};

// 円の実装
struct DraggableCircle : IDraggable
{
	Circle shape;
	Circle initialShape;
	P2Body body;
	bool isDragging = false;
	Vec2 dragOffset;

	//オブジェクトの初期状態を保存（リセット機構のため）
	P2World& world;

	DraggableCircle(const Circle& c, P2World& world)
		: shape(c), initialShape(c), world(world)
	{
		body = world.createCircle(P2Dynamic, c.center, c.r);
	}
	//オブジェクトにマウスカーソルがあるか判断して動かす関数
	void update() override
	{
		//マウスカーソルがオブジェクト内にありクリックされているかの判定
		if (!isDragging && shape.contains(Cursor::Pos()) && MouseL.down())
		{
			isDragging = true;
			dragOffset = Cursor::Pos() - shape.center;
			body = world.createCircle(P2Kinematic, shape.center, shape.r);
			//Dynamicbody.setAwake(false);
			//body.setAwake(true);
		}
		//クリック状態での処理
		if (isDragging)
		{

			//クリックを離した時
			if (MouseL.up())
			{
				isDragging = false;
				//body.setAwake(true);
				body = world.createCircle(P2Dynamic, shape.center, shape.r);
			}

			//クリックを押し続けている時
			else
			{
				Vec2 newCenter = Cursor::Pos() - dragOffset;

				// 画面内に収める制限（clamp(制限したい値、最小値の座標、最大値の座標）
				const Rect sceneRect = Scene::Rect();
				newCenter.x = Clamp(newCenter.x, shape.r, sceneRect.w - shape.r);
				newCenter.y = Clamp(newCenter.y, shape.r, sceneRect.h - shape.r);

				//移動した先のオブジェクトの描写
				//shape.setCenter(newCenter);
				// 物理ボディを物理的に移動させる（瞬間移動ではなく速度で）
				//Vec2 velocity = (newCenter - body.getPos()) / Scene::DeltaTime();
				body.setPos(newCenter);
				shape.setCenter(body.getPos());
				//body.setVelocity(velocity);
				body.setAwake(true);
				//body = world.createCircle(P2Kinematic, initialShape.center, initialShape.r);
			}
		}
	}

	//マウスカーソルがオブジェクト内にあるとき色をかえる関数
	void draw() const override
	{
		shape.draw(shape.contains(Cursor::Pos()) ? ColorF(Palette::Skyblue, 0.5) : ColorF(Palette::Skyblue));
	}

	//リセットするときの関数
	void reset() override
	{
		shape = initialShape;
		body.setPos(initialShape.center);
		body.setVelocity(Vec2{ 0, 0 });
		body.setAwake(false);
	}
};

// 四角形の実装
struct DraggableRect : IDraggable
{
	Rect shape;
	Rect initialShape;
	P2Body body;
	bool isDragging = false;
	Vec2 dragOffset;

	P2World& world;

	DraggableRect(const Rect& r, P2World& world)
		: shape(r), initialShape(r), world(world)
	{
		body = world.createRect(P2Dynamic, r.center(), r.size);
	}

	void update() override
	{
		if (!isDragging && shape.contains(Cursor::Pos()) && MouseL.down())
		{
			isDragging = true;
			dragOffset = Cursor::Pos() - shape.center();
			//body.setAwake(false);
			//body.setAwake(true);
			body = world.createRect(P2Kinematic, shape.center(), shape.size);
		}

		if (isDragging)
		{
			if (MouseL.up())
			{
				isDragging = false;
				//body.setAwake(true);
				body = world.createRect(P2Dynamic, shape.center(), shape.size);
			}
			else
			{
				Vec2 newCenter = Cursor::Pos() - dragOffset;

				// 画面内に収める制限
				const Rect sceneRect = Scene::Rect();
				newCenter.x = Clamp(newCenter.x, shape.w / 2.0, sceneRect.w - shape.w / 2.0);
				newCenter.y = Clamp(newCenter.y, shape.h / 2.0, sceneRect.h - shape.h / 2.0);

				// 物理ボディを物理的に移動させる（瞬間移動ではなく速度で）
				//Vec2 velocity = (newCenter - body.getPos()) / Scene::DeltaTime();
				//body.setVelocity(velocity);
				body.setPos(newCenter); // 物理ボディの位置を直接更新
				shape = Rect{ (newCenter - shape.size / 2).asPoint(), shape.size };
				//body.setAwake(true);
				//body = world.createRect(P2Kinematic, shape.center(), shape.size);
			}
		}
	}

	void draw() const override
	{
		shape.draw(shape.contains(Cursor::Pos()) ? ColorF(Palette::Lightgreen, 0.5) : ColorF(Palette::Lightgreen));
	}

	void reset() override
	{
		shape = initialShape;
		body.setPos(initialShape.center());
		body.setVelocity(Vec2{ 0, 0 });
		body.setAwake(false);
	}
};

// 物理エンジン
struct MyBody
{
	P2Body body;
	double radius;
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

//ゲームオーバーシーン
class GameOver : public App::Scene
{
public:
	GameOver(const InitData& init) : IScene{ init }
	{
		Scene::SetBackground(ColorF{ 0.2, 0.2, 0.2 });
	}

	void update() override
	{
		if (Button(Rect{ 300, 400, 200, 80 }, m_font, U"Restart", true))
		{
			changeScene(State::Tutorial);
		}
	}

	void draw() const override
	{
		m_font(U"Game Over").drawAt(Scene::Center(), Palette::Red);
	}

private:
	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
};

//クリアシーン
class Clear : public App::Scene
{
public:
	Clear(const InitData& init) : IScene{ init }
	{
		Scene::SetBackground(ColorF{ 0.8, 1.0, 0.8 });
	}

	void update() override
	{
		if (Button(Rect{ 300, 400, 200, 80 }, m_font, U"Title", true))
		{
			changeScene(State::Title);
		}
	}

	void draw() const override
	{
		m_font(U"Clear!").drawAt(Scene::Center(), Palette::Green);
	}

private:
	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
};


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
			// チュートリアルのシーンに移動
			changeScene(State::Tutorial);
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
		m_font(U"針落").draw(80, Vec2{ 320, 150 }, ColorF{ 0.2 });

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
		m_font(U"Seiya").draw(32, Vec2{ 100, 140 }, ColorF{ 0.0 });
		m_font(U"プログラマー").draw(32, Vec2{ 80, 200 }, ColorF{ 0.0 });
		m_font(U"Seiya").draw(32, Vec2{ 100, 240 }, ColorF{ 0.0 });
		m_font(U"bukinyan").draw(32, Vec2{ 200, 240 }, ColorF{ 0.0 });
		m_font(U"kanaka").draw(32, Vec2{ 350, 240 }, ColorF{ 0.0 });
		m_font(U"使用素材").draw(32, Vec2{ 80, 300 }, ColorF{ 0.0 });
		m_font(U"illustAC: https://www.ac-illust.com/").draw(32, Vec2{ 100, 340 }, ColorF{ 0.0 });
	}
private:
	const Font m_font{ FontMethod::MSDF, 32 };
};

// ステージ1
class Tutorial : public App::Scene
{
public:

	Tutorial(const InitData& init)
		: IScene{ init }
		, needle(U"example/needle.png")
		, camera{ Vec2{ 0, -300 }, 1.0 }
		, accumulatedTime(0.0)
	{
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });

		// 設置オブジェクト
		objects.push_back(std::make_shared<DraggableCircle>(Circle{ 115, 300, 30 }, world));
		objects.push_back(std::make_shared<DraggableRect>(Rect{ 85, 390, 60, 60 }, world));
		objects.push_back(std::make_shared<DraggableCircle>(Circle{ 115, 540, 40 }, world));

		// 物理ボディ初期化（針を後で落とす）
		isLaunched = false;

		// 地面（固定）
		grounds << world.createLine(P2Static, Vec2{ 0, 0 }, Line{ -50, -150, -300, -50 });
		grounds << world.createLineString(P2Static, Vec2{ 0, 0 },
			LineString{ Vec2{ 100, -50 }, Vec2{ 200, -50 }, Vec2{ 600, -150 } });
	}

	void update() override
	{
		// --- ボタン類 ---

		// 戻るボタン
		if (Button(Rect{ 10, 10, 200, 70 }, m_font, U"BackMenu", true))
		{
			getData().unlockedStage1 = true;
			changeScene(State::Title);
		}

		// リセットボタン
		if (Button(Rect{ 10, 90, 200, 70 }, m_font, U"ReSet", true))
		{
			for (auto& obj : objects)
			{
				obj->reset();
			}
			bodies.clear();
			isLaunched = false;
		}

		// 針を落とすボタン
		if (SimpleGUI::Button(U"針を落とす", Vec2{ 10, 180 }, 200.0))
		{
			if (!isLaunched)
			{
				isLaunched = true;
				double radius = 10.0;
				bodies << MyBody{
					world.createRect(P2Dynamic, Vec2{ 300, -400 }, Vec2{ 10, 120 }),
					radius
				};
			}
		}
	   
		// 停止判定
		bool isGameOver = false;
		static double stopTime = 0.0;
		for (auto& b : bodies)
		{
			if (b.body.getVelocity().length() < 1.0)
			{
				stopTime += Scene::DeltaTime();
				if (stopTime > 2.0)
				{
						   
					isGameOver= true;
					stopTime = 0.0;
				}
			}
			else
			{
				stopTime = 0.0;
			}
				   
		// 画面外判定
			if (b.body.getPos().y > Scene::Height() - 600) // 少し余裕を持たせる
			{
				isGameOver = true;
				break;
			}
		}
		if (isGameOver)
		{
			changeScene(State::GameOver);
		}
		//デバック用
		//Print << isGameOver;
		//Print << stopTime;
		
		//ゴール判定部分
		
		
		//if (条件)
		//{
			//getData().unlockedStage1 = true;
			//changeScene(State::Clear);
		//}

		// --- スクロール処理 ---
		const double contentHeight = lines.size() * 40.0;
		const double viewHeight = Scene::Height();
		const double topMargin = 50.0;
		const double maxScroll = (contentHeight > viewHeight)
			? (contentHeight - viewHeight + topMargin)
			: 0.0;

		scrollY += Mouse::Wheel() * scrollSpeed;
		scrollY = Clamp(scrollY, 0.0, maxScroll);

		for (int i = 0; i < lines.size(); ++i)
		{
			scrollFont(lines[i]).draw(250, 50 + i * 40 - scrollY);
		}

		// --- 設置物と背景描画 ---
		Rect{ 65, 250, 100, 100 }.draw(ColorF(0.9, 0.9, 1.0, 0.5));
		Rect{ 65, 370, 100, 100 }.draw(ColorF(0.9, 0.9, 1.0, 0.5));
		Rect{ 65, 490, 100, 100 }.draw(ColorF(0.9, 0.9, 1.0, 0.5));
		Rect{ 230, 0, 10, 600 }.draw(ColorF(0));

		// オブジェクト更新と描画
		for (auto& obj : objects)
		{
			obj->update();
			obj->draw();
		}

		// --- 物理演算 ---
		//if (isLaunched)
		//{
		accumulatedTime += Scene::DeltaTime();
		while (accumulatedTime >= StepTime)
		{
			world.update(StepTime);
			accumulatedTime -= StepTime;
		}
		//}

			// --- 描画 ---
		const auto t = camera.createTransformer();

		for (const auto& g : grounds)
		{
			g.draw(Palette::Gray);
		}

		for (const auto& b : bodies)
		{
			needle.scaled(0.2).rotated(b.body.getAngle()).drawAt(b.body.getPos());
		}
	}

private:

	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Texture needle;

	// スクロール関連
	double scrollY = 0.0;
	const double scrollSpeed = 40.0;
	const Font scrollFont{ 30 };
	Array<String> lines;

	// 設置オブジェクト
	Array<std::shared_ptr<IDraggable>> objects;

	// 物理関連
	static constexpr double StepTime = 1.0 / 200.0;
	double accumulatedTime;
	P2World world;
	Array<MyBody> bodies;
	Array<P2Body> grounds;
	Camera2D camera;

	bool isLaunched = false;
};

// ステージ1
class Stage1 : public App::Scene
{
public:

	Stage1(const InitData& init)
		: IScene{ init }
		, needle(U"example/needle.png")
		, camera{ Vec2{0, -300 }, 1.0 }
		, accumulatedTime(0.0)
	{
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });
		// 円と四角形を追加
		objects.push_back(std::make_shared<DraggableCircle>(Circle{ 105, 235, 40 }, world));
		objects.push_back(std::make_shared<DraggableRect>(Rect{ 65, 340, 80, 80 }, world));
		objects.push_back(std::make_shared<DraggableCircle>(Circle{ 105, 515, 60 }, world));
		// 物理ボディ
		double radius = 10;
		bodies << MyBody{
			world.createRect(P2Dynamic, Vec2{-100, -300}, Vec2{10, 120}),
			radius
		};

		// 地面
		// grounds << world.createRect(P2Static, Vec2{ 0, -200 }, SizeF{ 600, 20 });
		grounds << world.createLine(P2Static, Vec2{ 0, 0 }, Line{ -50, -150, -300, -50 });
		grounds << world.createLineString(P2Static, Vec2{ 0, 0 },
		LineString{ Vec2{ 100, -50 }, Vec2{ 200, -50 }, Vec2{ 600, -150 } });
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
		if (Button(Rect{ 10, 90, 200, 70 }, m_font, U"ReSet", true))
		{
			// 処理内容
			for (auto& obj : objects)
			{
				obj->reset();
			}
		}
		// 設置物をおくところの背景
		Rect{ 100, 170, 130, 130 }.draw();
		Rect{ 100, 310, 130, 130 }.draw();
		Rect{ 100, 450, 130, 130 }.draw();

		// 境界線ようの縦線
		Rect{ 230, 0, 10, 600 }.draw(ColorF{ 0 });
		
		// 停止判定
		bool isGameOver = false;
		static double stopTime = 0.0;
		for (auto& b : bodies)
		{
			if (b.body.getVelocity().length() < 1.0)
			{
				stopTime += Scene::DeltaTime();
				if (stopTime > 2.0)
				{
						   
					isGameOver= true;
				}
			}
			else
			{
				stopTime = 0.0;
			}
				   
		// 画面外判定
			if (b.body.getPos().y > Scene::Height() - 600) // 少し余裕を持たせる
			{
				isGameOver = true;
				break;
			}
		}
		if (isGameOver)
		{
			changeScene(State::GameOver);
		}
		//ゴール判定部分
		
		//if (条件)
		//{
			//getData().unlockedStage1 = true;
			//changeScene(State::Clear);
		//}
			

		// --- スクロール関連の定数を計算 ---

		// 全コンテンツの高さを計算 (行数 × 1行の高さ)
		const double contentHeight = lines.size() * 40.0;

		// 画面（表示領域）の高さを取得
		const double viewHeight = Scene::Height();

		// テキストの上側の余白
		const double topMargin = 50.0;

		// スクロール可能な最大量を計算
		// (コンテンツが画面より大きい場合のみ、スクロール量が発生)
		const double maxScroll = (contentHeight > viewHeight) ? (contentHeight - viewHeight + topMargin) : 0.0;

		// --- 入力処理 ---

		// マウスホイールの移動量に応じてスクロール位置を更新
		scrollY += Mouse::Wheel() * scrollSpeed;

		// --- スクロールバーの計算 ---

		// スクロールバーの領域を定義 (画面右端、幅 20px)
		const RectF scrollbarArea(Scene::Width() - 20, 0, 20, Scene::Height());

		// スクロールバーのつまみ(サム)の Rect を用意
		RectF thumb(scrollbarArea.x, 0, scrollbarArea.w, 0);

		// コンテンツが画面より大きい場合のみ、つまみを計算して表示
		if (contentHeight > viewHeight)
		{
			// つまみの高さを 200px に固定
			thumb.h = 200;
			// 現在のスクロール位置(scrollY)を、つまみのY座標に変換
			thumb.y = scrollbarArea.y + scrollY / maxScroll * (scrollbarArea.h - thumb.h);
		}

		// スクロールバーに対するマウス操作
		if (scrollbarArea.leftClicked())
		{
			// スクロールバーの領域をクリックしたら、その位置につまみが移動するようにスクロール量を逆算
			const double newThumbY = Clamp(Cursor::Pos().y - thumb.h / 2, scrollbarArea.y, scrollbarArea.y + scrollbarArea.h - thumb.h);
			scrollY = (newThumbY - scrollbarArea.y) / (scrollbarArea.h - thumb.h) * maxScroll;
		}
		else if (thumb.leftPressed())
		{
			// つまみをドラッグしている間の処理
			const double mouseY = Cursor::Pos().y;
			// マウスのY座標に合わせてつまみを移動
			const double newThumbY = Clamp(mouseY - (mouseY - thumb.y), scrollbarArea.y, scrollbarArea.y + scrollbarArea.h - thumb.h);
			// つまみの位置からスクロール量を逆算
			scrollY = (newThumbY - scrollbarArea.y) / (scrollbarArea.h - thumb.h) * maxScroll;
		}

		// --- スクロール位置の最終調整 ---

		// スクロール量が 0.0 ～ maxScroll の範囲に収まるように制限
		scrollY = Clamp(scrollY, 0.0, maxScroll);

		// --- 描画処理 ---

		// テキストを描画 (スクロール位置 scrollY を引くことで、表示位置を動かす)
		for (int i = 0; i < lines.size(); ++i)
		{
			scrollFont(lines[i]).draw(250, 50 + i * 40 - scrollY);
		}

		// スクロールバーを描画 (コンテンツが画面より大きい場合のみ)
		if (contentHeight > viewHeight)
		{
			// スクロールバーの背景(トラック)を灰色で描画
			scrollbarArea.draw(ColorF(0.5));
			// つまみ(サム)を明るい灰色で描画
			thumb.draw(ColorF(0.9));
		}


		// Rキーで初期位置に戻す
		if (KeyR.down())
		{
			for (auto& obj : objects)
			{
				obj->reset();
			}
		}

		// 更新と描画
		for (auto& obj : objects)
		{
			obj->update();
			obj->draw();
		}

		ClearPrint();

		// 情報表示
		for (const auto& b : bodies)
		{
			Print << U"ID: {}, Pos: {:.1f}"_fmt(b.body.id(), b.body.getPos());
		}

		// 物理更新
		accumulatedTime += Scene::DeltaTime();
		while (accumulatedTime >= StepTime)
		{
			world.update(StepTime);
			accumulatedTime -= StepTime;

			// 落下物の削除
			for (size_t i = 0; i < bodies.size(); )
			{
				if (bodies[i].body.getPos().y > 500)
				{
					bodies.remove_at(i);
				}
				else
				{
					++i;
				}
			}
		}

		// カメラ更新
		camera.update();
		const auto t = camera.createTransformer();

		// テクスチャ確認
		if (!needle)
		{
			Print << U"Texture 読み込み失敗";
		}

		// --- 描画 ---

		// 地面
		for (const auto& g : grounds)
		{
			g.draw(Palette::Gray);
		}

		// 動く物体
		for (const auto& b : bodies)
		{
			double scale = (b.radius * 2.0) / needle.width();
			needle.scaled(0.2).rotated(b.body.getAngle()).drawAt(b.body.getPos());
		}
	}

private:

	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Texture needle;
	// --- スクロール関連 ---
	double scrollY = 0.0;
	const double scrollSpeed = 40.0;
	const Font scrollFont{ 30 };
	Array<String> lines;
	// 設置物
	Array<std::shared_ptr<IDraggable>> objects;
	// 物理関連
	constexpr static double StepTime = 1.0 / 200.0;
	double accumulatedTime;
	P2World world;
	Array<MyBody> bodies;
	Array<P2Body> grounds;
	Camera2D camera;
};

// ステージ2
class Stage2 : public App::Scene
{
public:

	Stage2(const InitData& init)
		: IScene{ init }
		, needle(U"example/needle.png")
		, camera{ Vec2{0, -300 }, 1.0 }
		, accumulatedTime(0.0)
	{
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });

		// 円と四角形を追加
		objects.push_back(std::make_shared<DraggableCircle>(Circle{ 105, 235, 40 }, world));
		objects.push_back(std::make_shared<DraggableRect>(Rect{ 65, 340, 80, 80 }, world));
		objects.push_back(std::make_shared<DraggableCircle>(Circle{ 105, 515, 60 }, world));
		// 物理ボディ
		double radius = 10;
		bodies << MyBody{
			world.createRect(P2Dynamic, Vec2{-100, -300}, Vec2{10, 120}),
			radius
		};

		// 地面
		// grounds << world.createRect(P2Static, Vec2{ 0, -200 }, SizeF{ 600, 20 });
		grounds << world.createLine(P2Static, Vec2{ 0, 0 }, Line{ -50, -150, -300, -50 });
		grounds << world.createLineString(P2Static, Vec2{ 0, 0 },
		LineString{ Vec2{ 100, -50 }, Vec2{ 200, -50 }, Vec2{ 600, -150 } });
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
		if (Button(Rect{ 10, 90, 200, 70 }, m_font, U"ReSet", true))
		{
			// 処理内容
			for (auto& obj : objects)
			{
				obj->reset();
			}
		}
		// 設置物をおくところの背景
		Rect{ 40, 170, 130, 130 }.draw();
		Rect{ 40, 310, 130, 130 }.draw();
		Rect{ 40, 450, 130, 130 }.draw();

		// 境界線ようの縦線
		Rect{ 230, 0, 10, 600 }.draw(ColorF{ 0 });

		// --- スクロール関連の定数を計算 ---

		// 全コンテンツの高さを計算 (行数 × 1行の高さ)
		const double contentHeight = lines.size() * 40.0;

		// 画面（表示領域）の高さを取得
		const double viewHeight = Scene::Height();

		// テキストの上側の余白
		const double topMargin = 50.0;

		// スクロール可能な最大量を計算
		// (コンテンツが画面より大きい場合のみ、スクロール量が発生)
		const double maxScroll = (contentHeight > viewHeight) ? (contentHeight - viewHeight + topMargin) : 0.0;

		// --- 入力処理 ---

		// マウスホイールの移動量に応じてスクロール位置を更新
		scrollY += Mouse::Wheel() * scrollSpeed;

		// --- スクロールバーの計算 ---

		// スクロールバーの領域を定義 (画面右端、幅 20px)
		const RectF scrollbarArea(Scene::Width() - 20, 0, 20, Scene::Height());

		// スクロールバーのつまみ(サム)の Rect を用意
		RectF thumb(scrollbarArea.x, 0, scrollbarArea.w, 0);

		// コンテンツが画面より大きい場合のみ、つまみを計算して表示
		if (contentHeight > viewHeight)
		{
			// つまみの高さを 200px に固定
			thumb.h = 200;
			// 現在のスクロール位置(scrollY)を、つまみのY座標に変換
			thumb.y = scrollbarArea.y + scrollY / maxScroll * (scrollbarArea.h - thumb.h);
		}

		// スクロールバーに対するマウス操作
		if (scrollbarArea.leftClicked())
		{
			// スクロールバーの領域をクリックしたら、その位置につまみが移動するようにスクロール量を逆算
			const double newThumbY = Clamp(Cursor::Pos().y - thumb.h / 2, scrollbarArea.y, scrollbarArea.y + scrollbarArea.h - thumb.h);
			scrollY = (newThumbY - scrollbarArea.y) / (scrollbarArea.h - thumb.h) * maxScroll;
		}
		else if (thumb.leftPressed())
		{
			// つまみをドラッグしている間の処理
			const double mouseY = Cursor::Pos().y;
			// マウスのY座標に合わせてつまみを移動
			const double newThumbY = Clamp(mouseY - (mouseY - thumb.y), scrollbarArea.y, scrollbarArea.y + scrollbarArea.h - thumb.h);
			// つまみの位置からスクロール量を逆算
			scrollY = (newThumbY - scrollbarArea.y) / (scrollbarArea.h - thumb.h) * maxScroll;
		}

		// --- スクロール位置の最終調整 ---

		// スクロール量が 0.0 ～ maxScroll の範囲に収まるように制限
		scrollY = Clamp(scrollY, 0.0, maxScroll);

		// --- 描画処理 ---

		// テキストを描画 (スクロール位置 scrollY を引くことで、表示位置を動かす)
		for (int i = 0; i < lines.size(); ++i)
		{
			scrollFont(lines[i]).draw(250, 50 + i * 40 - scrollY);
		}

		// スクロールバーを描画 (コンテンツが画面より大きい場合のみ)
		if (contentHeight > viewHeight)
		{
			// スクロールバーの背景(トラック)を灰色で描画
			scrollbarArea.draw(ColorF(0.5));
			// つまみ(サム)を明るい灰色で描画
			thumb.draw(ColorF(0.9));
		}

		// Rキーで初期位置に戻す
		if (KeyR.down())
		{
			for (auto& obj : objects)
			{
				obj->reset();
			}
		}

		// 更新と描画
		for (auto& obj : objects)
		{
			obj->update();
			obj->draw();
		}

		ClearPrint();

		// 情報表示
		for (const auto& b : bodies)
		{
			Print << U"ID: {}, Pos: {:.1f}"_fmt(b.body.id(), b.body.getPos());
		}

		// 物理更新
		accumulatedTime += Scene::DeltaTime();
		while (accumulatedTime >= StepTime)
		{
			world.update(StepTime);
			accumulatedTime -= StepTime;

			// 落下物の削除
			for (size_t i = 0; i < bodies.size(); )
			{
				if (bodies[i].body.getPos().y > 500)
				{
					bodies.remove_at(i);
				}
				else
				{
					++i;
				}
			}
		}

		// カメラ更新
		camera.update();
		const auto t = camera.createTransformer();

		// テクスチャ確認
		if (!needle)
		{
			Print << U"Texture 読み込み失敗";
		}

		// --- 描画 ---

		// 地面
		for (const auto& g : grounds)
		{
			g.draw(Palette::Gray);
		}

		// 動く物体
		for (const auto& b : bodies)
		{
			double scale = (b.radius * 2.0) / needle.width();
			needle.scaled(0.2).rotated(b.body.getAngle()).drawAt(b.body.getPos());
		}
	}

private:

	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Texture needle;
	// --- スクロール関連 ---
	double scrollY = 0.0;
	const double scrollSpeed = 40.0;
	const Font scrollFont{ 30 };
	Array<String> lines;
	// 設置物
	Array<std::shared_ptr<IDraggable>> objects;
	// 物理関連
	constexpr static double StepTime = 1.0 / 200.0;
	double accumulatedTime;
	P2World world;
	Array<MyBody> bodies;
	Array<P2Body> grounds;
	Camera2D camera;
};

//　ステージ3
class Stage3 : public App::Scene
{
public:

	Stage3(const InitData& init)
		: IScene{ init }
		, needle(U"example/needle.png")
		, camera{ Vec2{0, -300 }, 1.0 }
		, accumulatedTime(0.0)
	{
		Scene::SetBackground(ColorF{ 0.7, 0.9, 1.0 });

		// 円と四角形を追加
		objects.push_back(std::make_shared<DraggableCircle>(Circle{ 105, 235, 40 }, world));
		objects.push_back(std::make_shared<DraggableRect>(Rect{ 65, 340, 80, 80 }, world));
		objects.push_back(std::make_shared<DraggableCircle>(Circle{ 105, 515, 60 }, world));
		// 物理ボディ
		double radius = 10;
		bodies << MyBody{
			world.createRect(P2Dynamic, Vec2{-100, -300}, Vec2{10, 120}),
			radius
		};

		// 地面
		// grounds << world.createRect(P2Static, Vec2{ 0, -200 }, SizeF{ 600, 20 });
		grounds << world.createLine(P2Static, Vec2{ 0, 0 }, Line{ -50, -150, -300, -50 });
		grounds << world.createLineString(P2Static, Vec2{ 0, 0 },
		LineString{ Vec2{ 100, -50 }, Vec2{ 200, -50 }, Vec2{ 600, -150 } });
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
		if (Button(Rect{ 10, 90, 200, 70 }, m_font, U"ReSet", true))
		{
			// 処理内容
			for (auto& obj : objects)
			{
				obj->reset();
			}
		}
		// 設置物をおくところの背景
		Rect{ 40, 170, 130, 130 }.draw();
		Rect{ 40, 310, 130, 130 }.draw();
		Rect{ 40, 450, 130, 130 }.draw();

		// 境界線ようの縦線
		Rect{ 230, 0, 10, 600 }.draw(ColorF{ 0 });

		// --- スクロール関連の定数を計算 ---

		// 全コンテンツの高さを計算 (行数 × 1行の高さ)
		const double contentHeight = lines.size() * 40.0;

		// 画面（表示領域）の高さを取得
		const double viewHeight = Scene::Height();

		// テキストの上側の余白
		const double topMargin = 50.0;

		// スクロール可能な最大量を計算
		// (コンテンツが画面より大きい場合のみ、スクロール量が発生)
		const double maxScroll = (contentHeight > viewHeight) ? (contentHeight - viewHeight + topMargin) : 0.0;

		// --- 入力処理 ---

		// マウスホイールの移動量に応じてスクロール位置を更新
		scrollY += Mouse::Wheel() * scrollSpeed;

		// --- スクロールバーの計算 ---

		// スクロールバーの領域を定義 (画面右端、幅 20px)
		const RectF scrollbarArea(Scene::Width() - 20, 0, 20, Scene::Height());

		// スクロールバーのつまみ(サム)の Rect を用意
		RectF thumb(scrollbarArea.x, 0, scrollbarArea.w, 0);

		// コンテンツが画面より大きい場合のみ、つまみを計算して表示
		if (contentHeight > viewHeight)
		{
			// つまみの高さを 200px に固定
			thumb.h = 200;
			// 現在のスクロール位置(scrollY)を、つまみのY座標に変換
			thumb.y = scrollbarArea.y + scrollY / maxScroll * (scrollbarArea.h - thumb.h);
		}

		// スクロールバーに対するマウス操作
		if (scrollbarArea.leftClicked())
		{
			// スクロールバーの領域をクリックしたら、その位置につまみが移動するようにスクロール量を逆算
			const double newThumbY = Clamp(Cursor::Pos().y - thumb.h / 2, scrollbarArea.y, scrollbarArea.y + scrollbarArea.h - thumb.h);
			scrollY = (newThumbY - scrollbarArea.y) / (scrollbarArea.h - thumb.h) * maxScroll;
		}
		else if (thumb.leftPressed())
		{
			// つまみをドラッグしている間の処理
			const double mouseY = Cursor::Pos().y;
			// マウスのY座標に合わせてつまみを移動
			const double newThumbY = Clamp(mouseY - (mouseY - thumb.y), scrollbarArea.y, scrollbarArea.y + scrollbarArea.h - thumb.h);
			// つまみの位置からスクロール量を逆算
			scrollY = (newThumbY - scrollbarArea.y) / (scrollbarArea.h - thumb.h) * maxScroll;
		}

		// --- スクロール位置の最終調整 ---

		// スクロール量が 0.0 ～ maxScroll の範囲に収まるように制限
		scrollY = Clamp(scrollY, 0.0, maxScroll);

		// --- 描画処理 ---

		// テキストを描画 (スクロール位置 scrollY を引くことで、表示位置を動かす)
		for (int i = 0; i < lines.size(); ++i)
		{
			scrollFont(lines[i]).draw(250, 50 + i * 40 - scrollY);
		}

		// スクロールバーを描画 (コンテンツが画面より大きい場合のみ)
		if (contentHeight > viewHeight)
		{
			// スクロールバーの背景(トラック)を灰色で描画
			scrollbarArea.draw(ColorF(0.5));
			// つまみ(サム)を明るい灰色で描画
			thumb.draw(ColorF(0.9));
		}


		// Rキーで初期位置に戻す
		if (KeyR.down())
		{
			for (auto& obj : objects)
			{
				obj->reset();
			}
		}

		// 更新と描画
		for (auto& obj : objects)
		{
			obj->update();
			obj->draw();
		}

		ClearPrint();

		// 情報表示
		for (const auto& b : bodies)
		{
			Print << U"ID: {}, Pos: {:.1f}"_fmt(b.body.id(), b.body.getPos());
		}

		// 物理更新
		accumulatedTime += Scene::DeltaTime();
		while (accumulatedTime >= StepTime)
		{
			world.update(StepTime);
			accumulatedTime -= StepTime;

			// 落下物の削除
			for (size_t i = 0; i < bodies.size(); )
			{
				if (bodies[i].body.getPos().y > 500)
				{
					bodies.remove_at(i);
				}
				else
				{
					++i;
				}
			}
		}

		// カメラ更新
		camera.update();
		const auto t = camera.createTransformer();

		// テクスチャ確認
		if (!needle)
		{
			Print << U"Texture 読み込み失敗";
		}

		// --- 描画 ---

		// 地面
		for (const auto& g : grounds)
		{
			g.draw(Palette::Gray);
		}

		// 動く物体
		for (const auto& b : bodies)
		{
			double scale = (b.radius * 2.0) / needle.width();
			needle.scaled(0.2).rotated(b.body.getAngle()).drawAt(b.body.getPos());
		}
	}

private:

	const Font m_font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Texture needle;
	// --- スクロール関連 ---
	double scrollY = 0.0;
	const double scrollSpeed = 40.0;
	const Font scrollFont{ 30 };
	Array<String> lines;
	// 設置物
	Array<std::shared_ptr<IDraggable>> objects;
	// 物理関連
	constexpr static double StepTime = 1.0 / 200.0;
	double accumulatedTime;
	P2World world;
	Array<MyBody> bodies;
	Array<P2Body> grounds;
	Camera2D camera;
};


void Main()
{
	// シーンマネージャーを作成
	App manager;

	// 各シーンを登録
	manager.add<Title>(State::Title);
	manager.add<Credit>(State::Credit);
	manager.add<Tutorial>(State::Tutorial);
	manager.add<Stage1>(State::Stage1);
	manager.add<Stage2>(State::Stage2);
	manager.add<Stage3>(State::Stage3);
	manager.add<GameOver>(State::GameOver);
	manager.add<Clear>(State::Clear);


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

