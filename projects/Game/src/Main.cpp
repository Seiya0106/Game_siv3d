# include <Siv3D.hpp>

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
		.drawShadow(Vec2{2, 2}, 12, 0)
		.draw(ColorF{0.9, 0.8, 0.6});
	
	//　枠を描く
	rect.stretched(-3).rounded(3)
		.drawFrame(2, ColorF{0.4, 0.3, 0.2});
	
	// テキストを描く
	font(text).drawAt(40, rect.center(), ColorF{0.4, 0.3, 0.2});
	
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

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	
	while (System::Update())
	{
		// タイトル
		font(U"Title").draw(80, Vec2{300, 150}, ColorF{0.2});
		
		// Credit
		if (Button(Rect{10, 10, 150, 80}, font, U"Credit", true))
		{
			Print << U"Credit";
		}
		
		// Tutorial
		if (Button(Rect{250, 280, 300, 60}, font, U"Tutorial", true))
		{
			Print << U"Tutorial";
		}
		
		// Stage1
		if (Button(Rect{80, 400, 200, 80}, font, U"Stage1", false))
		{
			Print << U"Stage1";
		}
		// Stage2
		if (Button(Rect{300, 400, 200, 80}, font, U"Stage2", false))
		{
			Print << U"Stage2";
		}
		if (Button(Rect{520, 400, 200, 80}, font,U"Stage3", false))
		{
			Print << U"Stage3";
		}
	}
}
