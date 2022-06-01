#pragma once

namespace tiny 
{
	class TinyEngine
	{
	public:
		void startEngine();
		void shutdownEngine();
		void run();

		void initialize();
		void clear();
	protected:
		void logicalTick(float delta_time);
		void rendererTick(float delta_time);
		
		void calculateFPS(float delta_time);
	};
}


