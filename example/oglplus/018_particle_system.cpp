/**
 *  @example oglplus/018_particle_system.cpp
 *  @brief Shows how to draw a simple particle system using the geometry shader
 *
 *  @image html 018_particle_system.png
 *
 *  Copyright 2008-2011 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <oglplus/gl.hpp>
#include <oglplus/all.hpp>

#include <vector>
#include <cmath>

#include "example.hpp"

namespace oglplus {

class ParticlesExample : public Example
{
private:
	// wrapper around the current OpenGL context
	Context gl;

	// Vertex shader
	VertexShader vs;

	// Geometry shader
	GeometryShader gs;

	// Fragment shader
	FragmentShader fs;

	// Program
	Program prog;

	const size_t particle_count;
	std::vector<Vec3f> positions;
	std::vector<Vec3f> directions;
	std::vector<float> ages;

	// A vertex array object for the particles
	VertexArray particles;

	// VBOs for the particle positions and ages
	Buffer pos_buf, age_buf;

	double prev_time, prev_spawn;
public:
	ParticlesExample(void)
	 : particle_count(100)
	 , prev_time(0.0)
	 , prev_spawn(0.0)
	{
		// Set the vertex shader source
		vs.Source(
			"#version 330\n"
			"uniform mat4 ModelMatrix, CameraMatrix;"
			"in vec4 Position;"
			"in float Age;"
			"out float vertAge;"
			"void main(void)"
			"{"
			"	gl_Position = "
			"		CameraMatrix *"
			"		ModelMatrix *"
			"		Position;"
			"	vertAge = Age;"
			"}"
		);
		// compile it
		vs.Compile();

		// Set the geometry shader source
		gs.Source(
			"#version 330\n"
			"layout(points) in;"
			"layout(triangle_strip, max_vertices = 4) out;"
			"uniform mat4 ProjectionMatrix;"
			"in float vertAge[];"
			"out float geomAge;"
			"void main(void)"
			"{"
			"	float s = 0.5;"
			"	float yo[2] = float[2](-1.0, 1.0);"
			"	float xo[2] = float[2](-1.0, 1.0);"
			"	for(int j=0;j!=2;++j)"
			"	for(int i=0;i!=2;++i)"
			"	{"
			"		float xoffs = xo[i]*(1.0+vertAge[0])*s;"
			"		float yoffs = yo[j]*(1.0+vertAge[0])*s;"
			"		gl_Position = ProjectionMatrix * vec4("
			"			gl_in[0].gl_Position.x-xoffs,"
			"			gl_in[0].gl_Position.y-yoffs,"
			"			gl_in[0].gl_Position.z,"
			"			1.0"
			"		);"
			"		geomAge = vertAge[0];"
			"		EmitVertex();"
			"	}"
			"	EndPrimitive();"
			"}"
		);
		// compile it
		gs.Compile();

		// set the fragment shader source
		fs.Source(
			"#version 330\n"
			"in float geomAge;"
			"out vec4 fragColor;"
			"void main(void)"
			"{"
			"	vec3 Color1 = vec3(1.0, 0.5, 0.5);"
			"	vec3 Color2 = vec3(0.3, 0.1, 0.1);"
			"	fragColor = vec4("
			"		mix(Color1, Color2, geomAge),"
			"		1.0 - geomAge"
			"	);"
			"}"
		);
		// compile it
		fs.Compile();

		// attach the shaders to the program
		prog.AttachShader(vs);
		prog.AttachShader(gs);
		prog.AttachShader(fs);
		// link and use it
		prog.Link();
		prog.Use();

		// bind the VAO for the particles
		particles.Bind();

		// bind the VBO for the particle positions
		pos_buf.Bind(Buffer::Target::Array);
		{
			Buffer::Data(Buffer::Target::Array, positions);
			VertexAttribArray attr(prog, "Position");
			attr.Setup(3, DataType::Float);
			attr.Enable();
		}
		positions.reserve(particle_count);
		directions.reserve(particle_count);

		// bind the VBO for the particle ages
		age_buf.Bind(Buffer::Target::Array);
		{
			Buffer::Data(Buffer::Target::Array, ages);
			VertexAttribArray attr(prog, "Age");
			attr.Setup(1, DataType::Float);
			attr.Enable();
		}
		ages.reserve(particle_count);

		Uniform(prog, "ModelMatrix").SetMatrix(
			ModelMatrixf::Translation(0.0f, -10.0f, 0.0f)
		);
		//
		gl.ClearColor(0.9f, 0.9f, 0.9f, 0.0f);
		gl.ClearDepth(1.0f);
		gl.Enable(Capability::DepthTest);
		gl.Enable(Capability::Blend);
		gl.BlendFunc(BlendFn::SrcAlpha, BlendFn::OneMinusSrcAlpha);
	}

	void Reshape(size_t width, size_t height)
	{
		gl.Viewport(width, height);
		prog.Use();
		Uniform(prog, "ProjectionMatrix").SetMatrix(
			CamMatrixf::Perspective(
				Degrees(24),
				double(width)/height,
				1, 100
			)
		);
	}

	// creates a directional vector for a new particle
	static Vec3f NewDirection(void)
	{
		float disp = 2.0f;
		float dx = (0.5f - (float(std::rand())/RAND_MAX))*disp;
		float dy = 5.0f + (0.5 - (float(std::rand())/RAND_MAX))*disp;;
		float dz = (0.5f - (float(std::rand())/RAND_MAX))*disp;
		return Vec3f(dx, dy, dz);
	}

	void Render(double time)
	{
		// update the particle positions, ages and directions
		size_t i = 0;
		float time_diff = (time - prev_time);
		float age_mult = 0.2f;
		while(i != positions.size())
		{
			float drag = 0.1f * (time_diff);
			if((ages[i] += time_diff * age_mult) < 1.0f)
			{
				directions[i] *= (1.0f - drag);
				positions[i] += directions[i]*time_diff;
			}
			else
			{
				ages[i] = 0.0f;
				directions[i] = NewDirection();
				positions[i] = Vec3f();
			}
			++i;
		}
		// if there are not enough particles yet
		if(i != particle_count)
		{
			float spawn_interval = 1.0f/(age_mult*particle_count);
			if(prev_spawn + spawn_interval < time)
			{
				directions.push_back(NewDirection());
				positions.push_back(Vec3f());
				ages.push_back(0.0f);
				prev_spawn = time;
			}
		}
		prev_time = time;

		assert(positions.size() == directions.size());
		assert(positions.size() == ages.size());

		// upload the particle positions
		pos_buf.Bind(Buffer::Target::Array);
		Buffer::Data(Buffer::Target::Array, positions);
		// upload the particle ages
		age_buf.Bind(Buffer::Target::Array);
		Buffer::Data(Buffer::Target::Array, ages);

		gl.Clear().ColorBuffer().DepthBuffer();
		//
		// set the matrix for camera orbiting the origin
		Uniform(prog, "CameraMatrix").SetMatrix(
			CamMatrixf::Orbiting(
				Vec3f(),
				12.0f,
				FullCircles(time * 0.5),
				Degrees(45)
			)
		);

		gl.DrawArrays(PrimitiveType::Points, 0, positions.size());
	}

	bool Continue(double time)
	{
		return time < 30.0;
	}
};

std::unique_ptr<Example> makeExample(void)
{
	return std::unique_ptr<Example>(new ParticlesExample);
}

} // namespace oglplus