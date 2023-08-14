#include <Novice.h>
#include <Function.h>
#include <Vector3.h>
#include <cstdint>
#include <imgui.h>

const char kWindowTitle[] = "LE2B_29_ヨシダ_タケル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };



	//Affine
	Vector3 scale = { 1.0f,1.0f,1.0f };
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	Vector3 translate = { 0.0f,0.0f,0.0f };

	//カメラ
	//Vector3 cameraPosition = { 0.0f,0.0f,-1.0f };
	Vector3 cameraTranslate = { 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };


	//ローカル
	Vector3 LocalVertics[2] = {};
	LocalVertics[0] = { -0.2f,0.0f,0.0f };
	LocalVertics[1] = { 0.2f,0.0f,0.0f };


	Vector3 localCoodinate = { 0.0f,0.0f,0.0f };



	Segment segment = { {-2.0f,-1.0f,0.1f},{3.0f,2.0f,2.0f} };
	Vector3 point = { 0.0f,0.6f,0.6f };

	Vector3 project = {};
	Vector3 closestPoint = {};

	//1cmの球を描画
	Sphere pointSphere = { point,0.1f };;
	Sphere closestPointSphere = { closestPoint,0.01f };;




	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///


		



		//計算
		Matrix4x4 cameraMatrix = MakeAffineMatrix(scale, cameraRotate, cameraTranslate);
		////ビュー(カメラ)
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);

		//射影
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WINDOW_SIZE_WIDTH) / float(WINDOW_SIZE_HEIGHT), 0.1f, 100.0f);


		//ビューポート
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(WINDOW_SIZE_WIDTH), float(WINDOW_SIZE_HEIGHT), 0.0f, 1.0f);




		//正射影ベクトルと最近接点
		project = Project(Subtract(point, segment.origin), segment.diff);
		closestPoint = ClosestPoint(point, segment);



		///
		/// ↑更新処理ここまで
		///




		///
		/// ↓描画処理ここから
		///

		//線分
		Matrix4x4 WorldMatrixSegmentOrigin = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, segment.origin);
		Matrix4x4 WorldMatrixSegmentDiff = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, segment.diff);


		Matrix4x4 worldViewProjectionMatrixSegmentOrigin = Multiply(WorldMatrixSegmentOrigin, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 worldViewProjectionMatrixSegmentDiff = Multiply(WorldMatrixSegmentDiff, Multiply(viewMatrix, projectionMatrix));


		Vector3 ndcVerticesSegmentOrigin = Transform(segment.origin, worldViewProjectionMatrixSegmentOrigin);
		Vector3 ndcVerticesSegmentDiff = Transform(segment.diff, worldViewProjectionMatrixSegmentDiff);


		Vector3 start = Transform(ndcVerticesSegmentOrigin, viewportMatrix);
		Vector3 end = Transform(ndcVerticesSegmentDiff, viewportMatrix);



		//Point(資料だとP)
		Matrix4x4 WorldMatrixPoint = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, point);

		Matrix4x4 worldViewProjectionMatrixPoint = Multiply(WorldMatrixPoint, Multiply(viewMatrix, projectionMatrix));

		Vector3 ndcVerticesPoint = Transform(point, worldViewProjectionMatrixPoint);

		Vector3 pointCoodinate = Transform(ndcVerticesPoint, viewportMatrix);


		//Projection
		Matrix4x4 WorldMatrixProject = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, project);


		Matrix4x4 worldViewProjectionMatrixProject = Multiply(WorldMatrixProject, Multiply(viewMatrix, projectionMatrix));


		Vector3 ndcVerticesProject = Transform(project, worldViewProjectionMatrixProject);


		Vector3 projectCoodinate = Transform(Add(ndcVerticesSegmentOrigin, ndcVerticesProject), viewportMatrix);





		Novice::DrawLine(
			int(start.x),
			int(start.y),
			int(end.x),
			int(end.y), WHITE);


#pragma endregion


		//Grid
		DrawGrid(viewMatrix, projectionMatrix, viewportMatrix);

		//赤の方座標がおかしい






		//SegmentOrigin
		Novice::DrawEllipse(
			int(start.x),
			int(start.y),
			int(50.0f),
			int(50.0f), 0.0f, BLUE, kFillModeSolid);


		//VectorOについて
		Novice::DrawLine(
			int(start.x),
			int(start.y),
			int(pointCoodinate.x),
			int(pointCoodinate.y), BLACK);

		//VectorOについて
		Novice::DrawEllipse(
			int(pointCoodinate.x),
			int(pointCoodinate.y),
			50, 50, 0.0f, RED, kFillModeSolid);


		//Proj
		//Localを入れるよ
		DrawSphere({ segment.diff,pointSphere.radius }, viewMatrix, projectionMatrix, viewportMatrix, RED);
		DrawSphere({ point,pointSphere.radius }, viewMatrix, projectionMatrix, viewportMatrix, BLACK);

		ImGui::Begin("Window");
		ImGui::DragFloat3("point", &point.x, 0.01f);
		ImGui::DragFloat3("Segment origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segmen difft", &segment.diff.x, 0.01f);

		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::End();




		ImGui::Begin("Camera");
		ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);

		ImGui::End();


		ImGui::Begin("Shere");
		ImGui::DragFloat3("Point", &project.x, 0.01f);
		ImGui::DragFloat3("ClosestPoint", &closestPoint.x, 0.01f);
		ImGui::SliderFloat("Radius", &pointSphere.radius, 0.0f, 0.5f);

		ImGui::End();



		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}