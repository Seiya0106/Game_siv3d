#include <Siv3D.hpp>

struct MyBody
{
    P2Body body;
    double radius;
};

void Main()
{
    Window::Resize(1280, 720);

    constexpr double StepTime = 1.0 / 200.0;
    double accumulatedTime = 0.0;

    // 2D物理世界
    P2World world;

    // テクスチャ読み込み
    Texture tex(U"example/needle.png");

    // ボディ作成（半径などを明示）
    Array<MyBody> bodies;
    {
        double radius = 10; // 例: 幅10
        bodies << MyBody{
            world.createRect(P2Dynamic, Vec2{-100, -300}, Vec2{10, 120}),
            radius
        };
    }

    // 地面（静的ボディ）
    Array<P2Body> grounds;
    //grounds << world.createRect(P2Static, Vec2{ 0, -200 }, SizeF{ 600, 20 });
    grounds << world.createLine(P2Static, Vec2{ 0, 0 }, Line{ -50, -150, -300, -50 });
    grounds << world.createLineString(P2Static, Vec2{ 0, 0 },
        LineString{ Vec2{ 100, -50 }, Vec2{ 200, -50 }, Vec2{ 600, -150 } });

    // カメラ
    Camera2D camera{ Vec2{0, -300}, 1.0 };

    while (System::Update())
    {
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
        if (!tex)
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
            double scale = (b.radius * 2.0) / tex.width();
            tex.scaled(0.2).rotated(b.body.getAngle()).drawAt(b.body.getPos());
        }
    }
}

