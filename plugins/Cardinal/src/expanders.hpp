
struct CardinalExpander
{
	CardinalExpander(){
	};
	virtual ~CardinalExpander(){};

	bool isCardinalExpandable(Module *x) { return x->model == modelIldaeil || x->model == modelCarla; };
	bool isCardinalExpander(Module *x) { return x->model == modelCardinalExpIn8; };

	virtual void sendExpMessage() = 0;
	virtual void processExpMessage() = 0;

	float rightMessages[2][8] = {}; // messages from right-side
    float leftMessages[2][8] = {};// messages from left-side

    void setupExpanding(Module *module) {
        module->leftExpander.producerMessage = leftMessages[0];
        module->leftExpander.consumerMessage = leftMessages[1];
    }
};