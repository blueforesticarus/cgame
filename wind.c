#define WIND_ROWS 8
#define WIND_COLUMNS 10
#define PRESURE_AIRSPEED_CONSTANT ?

typedef struct {
  double pressure;
  double fx, fy;
} WindNode;

WindNode nodes[WIND_ROWS*WIND_COLUMNS];

void update(){
	int row, col;
	double diff;
	//calculate airflow
	for(int i=0; i < WIND_ROWS*WIND_COLUMNS; i++){
		if(i / WIND_COLUMNS < WIND_ROWS - 1 && i % WIND_COLUMNS < WIND_COLUMNS - 1 ){
			//verticle
			diff = nodes[i].pressure - nodes[i+WIND_COLUMNS].pressure;				
			nodes[i].fy = diff * PRESURE_AIRSPEED_CONSTANT;		

			//horizontal	
			diff = nodes[i].pressure - nodes[i+1].pressure;				
			nodes[i].fx = diff * PRESURE_AIRSPEED_CONSTANT;
		}	
	}
	//update pressure
	for(int i=0; i < WIND_ROWS * WIND_COLUMNS; i++){
		nodes[i].pressure -= nodes[i].fy + nodes[i].fx;
		if(i > 0) nodes[i].pressure += nodes[i - 1].fx;
		if(i >= WIND_COLUMNS) nodes[i].pressure += nodes[i - WIND_COLUMNS].fy;
	}
}
