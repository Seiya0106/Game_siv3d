# include <Siv3D.hpp>

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
	bool isDragging = false;
	Vec2 dragOffset;

	DraggableCircle(const Circle& c) : shape(c), initialShape(c) {}

	void update() override
	{
		if (!isDragging && shape.contains(Cursor::Pos()) && MouseL.down())
		{
			isDragging = true;
			dragOffset = Cursor::Pos() - shape.center;
		}

		if (isDragging)
		{
			if (MouseL.up())
			{
				isDragging = false;
			}
			else
			{
				Vec2 newCenter = Cursor::Pos() - dragOffset;

				// 画面内に収める制限
				const Rect sceneRect = Scene::Rect();
				newCenter.x = Clamp(newCenter.x, shape.r, sceneRect.w - shape.r);
				newCenter.y = Clamp(newCenter.y, shape.r, sceneRect.h - shape.r);

				shape.setCenter(newCenter);
			}
		}
	}

	void draw() const override
	{
		shape.draw(shape.contains(Cursor::Pos()) ? ColorF(Palette::Skyblue, 0.5) : ColorF(Palette::Skyblue));
	}

	void reset() override
	{
		shape = initialShape;
	}
};

// 四角形の実装
struct DraggableRect : IDraggable
{
	Rect shape;
	Rect initialShape;
	bool isDragging = false;
	Vec2 dragOffset;

	DraggableRect(const Rect& r) : shape(r), initialShape(r) {}

	void update() override
	{
		if (!isDragging && shape.contains(Cursor::Pos()) && MouseL.down())
		{
			isDragging = true;
			dragOffset = Cursor::Pos() - shape.center();
		}

		if (isDragging)
		{
			if (MouseL.up())
			{
				isDragging = false;
			}
			else
			{
				Vec2 newCenter = Cursor::Pos() - dragOffset;
				
				// 画面内に収める制限
				const Rect sceneRect = Scene::Rect();
				newCenter.x = Clamp(newCenter.x, shape.w / 2.0, sceneRect.w - shape.w / 2.0);
				newCenter.y = Clamp(newCenter.y, shape.h / 2.0, sceneRect.h - shape.h / 2.0);
				
				// 中心から左上座標を計算して再構築
				shape = Rect{ (newCenter - shape.size / 2).asPoint(), shape.size };
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
	}
};

// メインの実行部分
void Main()
{
	Array<std::shared_ptr<IDraggable>> objects;

	// 円と四角形を追加
	objects.push_back(std::make_shared<DraggableCircle>(Circle{ 200, 200, 40 }));
	objects.push_back(std::make_shared<DraggableRect>(Rect{ 400, 300, 80, 80 }));
	objects.push_back(std::make_shared<DraggableCircle>(Circle{ 300, 300, 60 }));

	while (System::Update())
	{
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
	}
}


