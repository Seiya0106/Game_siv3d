# include <Siv3D.hpp>

bool Button(const Rect& rect, const Font& font, const String& text)
{
	// マウスカーソルがボタンの上にある場合
	if (rect.mouseOver())
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
		font(U"Title").draw(80, Vec2{300, 100}, ColorF{0.2});
		
		// Credit
		if (Button(Rect{10, 10, 150, 80}, font, U"Credit"))
		{
			Print << U"Credit";
		}
		
		// Stage1
		if (Button(Rect{80, 300, 300, 80}, font, U"Stage1"))
		{
			Print << U"Stage1";
		}
		// Stage2
		if (Button(Rect{420, 300, 300, 80}, font, U"Stage2"))
		{
			Print << U"Stage2";
		}
	}
}
