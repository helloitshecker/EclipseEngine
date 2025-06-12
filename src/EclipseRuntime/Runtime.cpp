#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <implot.h>
#include <implot_internal.h>

//#include <EclipseGL/GL.hpp>
#include <glad/glad.h>
#include <EclipseRuntime/Window.hpp>
#include <EclipseUtils/Logger.hpp>
#include <EclipseUtils/Maths.hpp>
#include <EclipseGL/Mesh.hpp>
#include <EclipseGL/Shader.hpp>
#include <EclipseGL/Texture.hpp>
#include <EclipseGL/Camera.hpp>
#include <EclipseFileSystem/ModelLoader.hpp>
#include <EclipseScene/SceneManager.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <array>

void static framebuffer_size_callback(et::window, int, int);
void static mouse_callback(et::window, double, double);

e::camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
const float cameraSpeed = 2.5f;
static bool cursorCapture = false;

double delta = 0;
bool vsync = true;
bool wireframe = false;
bool normal = false;

glm::vec3 obj_color = { 1.0f, 1.0f, 1.0f };
glm::vec3 light_color = { 1.0f, 1.0f, 1.0f };
glm::vec3 light_pos = { 1006.0f, 1668.0f, -353.0f };

std::vector<double> frametimes;
const size_t max_points = 100;

e::window* pwindow;

void static framebuffer_size_callback(et::window _w, int width, int height) {
	e::window* window = pwindow;
	glViewport(0, 0, width, height);
	window->window_height = height;
	window->window_width = width;
}

static void mouse_callback(et::window _w, double xposIn, double yposIn) {
	e::window* window = pwindow;
	static bool firstMouse = true;
	static float lastX = window->window_width / 2.0;
	static float lastY = window->window_height / 2.0;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	if (cursorCapture)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

void static init_imgui(et::window _pwindow) {
	// ImGui Initialize
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("../assets/Poppins-Regular.ttf", 15);
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(_pwindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void static destroy_imgui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}

void static listen_to_input(e::window window) {
	static bool escapeKeyPressed = false;

	if (window.getKeyState(E_KEY_ESCAPE) == E_PRESS) {
		if (!escapeKeyPressed) {
			if (!cursorCapture) {
				glfwSetInputMode(window.pwindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode(window.pwindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			cursorCapture = !cursorCapture;
			escapeKeyPressed = true;
		}
	}
	else if (window.getKeyState(E_KEY_ESCAPE) == E_RELEASE) {
		escapeKeyPressed = false;
	}


	float deltaCameraSpeed = cameraSpeed * delta;
	if (window.getKeyState(E_KEY_W) == E_PRESS) camera.ProcessKeyboard(FORWARD, deltaCameraSpeed);
	if (window.getKeyState(E_KEY_A) == E_PRESS) camera.ProcessKeyboard(LEFT, deltaCameraSpeed);
	if (window.getKeyState(E_KEY_S) == E_PRESS) camera.ProcessKeyboard(BACKWARD, deltaCameraSpeed);
	if (window.getKeyState(E_KEY_D) == E_PRESS) camera.ProcessKeyboard(RIGHT, deltaCameraSpeed);
}

et::u32 static display_with_interval(et::u32 value, et::u32 interval) {
	static et::u32 interval_timer = 0;
	static et::u32 display_with_interval_previous_value = 0;
	interval_timer++;
	if (interval_timer > interval) {
		interval_timer = 0;
		display_with_interval_previous_value = value;
	}
	return display_with_interval_previous_value;
}

int main(int argc, char* argv[]) {
	// Fill create window struct
	e::window::WindowCreateInfo createInfo{ "Eclipse", 800, 600, true, false, vsync, 4, framebuffer_size_callback, mouse_callback }; 


	
	// Create window
	e::window window(createInfo);
	pwindow = &window;

	// Create OpenGL Context
	if (!gladLoadGLLoader((GLADloadproc)e::window::getProcAddress)) {
		LOG::SEND("Failed to find opengl context!");
	}

	// Set viewport
	glfwGetWindowSize(window.pwindow, &window.window_width, &window.window_height);
	glViewport(0, 0, window.window_width, window.window_height);

	// Enable MSAA
	glEnable(GL_MULTISAMPLE);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initalize ImGUI
	init_imgui(window.pwindow);

	// GL setup
	e::shader testshader("../src/Shaders/TestShader.v.glsl", "../src/Shaders/TestShader.f.glsl");
	e::shader normalshader("../src/Shaders/NormalDebug.v.glsl", "../src/Shaders/NormalDebug.f.glsl");

	e::scene mainscene(camera);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window.window_width / (float)window.window_height, 0.1f, 10000.0f);
	glm::mat4 view = mainscene.mainCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1);

	et::model model1 = e::loader::loadobj("../assets/teapot.obj");
	e::mesh mesh1(model1.vertices, model1.indices);
	e::material material1("Material 1", &testshader);

	material1.set("projection", &projection);
	material1.set("view", &view);
	material1.set("model", &model);
	material1.set("lightPos", &light_pos);
	material1.set("lightColor", &light_color);
	material1.set("objectColor", &obj_color);
	material1.set("viewPos", &camera.Position);

	et::object object{};
	object.name = "Kettle";
	object.mesh = &mesh1;
	object.transform = et::transform{ glm::vec3{0, 0, 0}, glm::vec3{0, 0, 0}, glm::vec3{1, 1, 1} };
	object.material = &material1;
	mainscene.push_object(object);

	//et::model model2 = e::loader::loadobj("../assets/bottle.obj");
	//e::mesh mesh2(model2.vertices, model2.indices);

	//et::object object2{};
	//object2.name = "Bottle";
	//object2.mesh = &mesh2;
	//object2.transform = et::transform{ glm::vec3{5, 5, 0}, glm::vec3{0, 0, 0}, glm::vec3{.2, .2, .2} };
	//mainscene.push_object(object2);

	glEnable(GL_DEPTH_TEST);

	// mainloop
	while (!window.shouldClose()) {
		// delta
		delta = window.getIntervalToPreviousCall();
		// Store frametime (e.g., in milliseconds)
		double ms = delta * 1000.0;
		frametimes.push_back(ms);
		if (frametimes.size() > max_points)
			frametimes.erase(frametimes.begin());
		
		// Input
		listen_to_input(window);

		// Clear framebuffer before draw
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Frame start call
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Camera setup
		projection = glm::perspective(glm::radians(camera.Zoom), (float)window.window_width / (float)window.window_height, 0.1f, 10000.0f);
		view = mainscene.mainCamera->GetViewMatrix();

		for (et::object& object : mainscene.objects) {
			model = object.transform.mat4();
			object.material->apply();
			object.mesh->draw();
		}
		
		// UI Layer 
		ImGui::Begin("Debugger");
		
		ImGui::Text("FPS: %d", display_with_interval(e::maths::ceil(1.0 / delta), 100));

		if (ImPlot::BeginPlot("Frametime Graph", ImVec2(-1, 150))) {
			// Optional: setup axes labels and automatic fitting
			ImPlot::SetupAxes("Frame", "ms", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_LockMin);

			// Build x-axis values as indices (0 to N-1)
			std::vector<double> xs(frametimes.size());
			for (size_t i = 0; i < frametimes.size(); ++i)
				xs[i] = static_cast<double>(i);

			ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImPlot::PlotLine("Frame Time", xs.data(), frametimes.data(), static_cast<int>(frametimes.size()));
			ImPlot::PopStyleColor();
			ImPlot::EndPlot();
		}

		ImGui::Checkbox("Vsync", &vsync);
		ImGui::Checkbox("Wireframe", &wireframe);
		ImGui::Checkbox("Normals", &normal);

		ImGui::ColorEdit3("Object Color", &obj_color[0]);
		ImGui::ColorEdit3("Light Color", &light_color[0]);
		ImGui::DragFloat3("Light Position", &light_pos[0]);

		ImGui::End();

		ImGui::Begin("Scene Hierarchy");

		for (et::object& object : mainscene.objects) {
			if (ImGui::TreeNode(object.name.c_str())) {
				ImGui::Text("Component: Transform");
				ImGui::DragFloat3("Location", &object.transform.position[0], 0.2f);
				ImGui::DragFloat3("Rotation", &object.transform.rotation[0], 0.2f);
				ImGui::DragFloat3("Scale", &object.transform.scale[0], 0.2f);
				ImGui::TreePop();
			}
		}

		ImGui::End();

		ImGui::Render();

		// Draw
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.setVsync(vsync);

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		window.update();
	}

	destroy_imgui();
}
