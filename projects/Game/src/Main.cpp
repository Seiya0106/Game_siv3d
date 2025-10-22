# include <Siv3D.hpp>

// 抽象的なインターフェース（IDraggable はインターフェース（抽象クラス）で、ドラッグ可能なオブジェクトの共通機能を定義。）
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
				shape.setCenter((Cursor::Pos() - dragOffset).asPoint());
			}
		}
	}

	void draw() const override
	{
		//条件?真の式:偽の式
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
				shape.setCenter((Cursor::Pos() - dragOffset).asPoint());
			}
		}
	}

	void draw() const override
	{   shape.draw(shape.contains(Cursor::Pos())? ColorF(Palette::Lightgreen, 0.5):ColorF(Palette::Lightgreen));
	}

	void reset() override
	{
		shape = initialShape;
	}
};
//メインの実行部分
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
			for (auto& obj : objects)       //objectsに入っている円と四角をobjとして取り出す、objの型をautoで自動推論している
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


