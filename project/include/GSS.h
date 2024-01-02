struct node {
    node* parent;
    int state;
};

class GSS {
    private:
        node* top;
        int pos;

    public:
        GSS() : top(nullptr), pos(0) {}

        int top_state() {
            return top->state;
        }

        int top_pos() {
            return pos;
        }

        void push(int value, int pos) {
            if (!top) {
                top = new node{nullptr, value};
                this->pos = pos;
            }
        }

        void pop() {
            node* to_delete = top;
            top = top->parent;
            delete to_delete;
        }
};
