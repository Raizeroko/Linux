#include <string>

enum{
    OP_ERROR = 2,
    DIVISION_ZERO
};

class Task
{
public:
    Task(int num1, char op, int num2)
        : _num1(num1)
        , _op(op)
        , _num2(num2)
    {}

    void Run()
    {
        _exit_code = 0;
        switch (_op)
        {
        case '+':
            _result = _num1 + _num2;
            break;
        case '-':
            _result = _num1 - _num2;
            break;
        case '*':
            _result = _num1 * _num2;
            break;
        case '/':
            if(_num2 == 0){
                _result = 0;
                _exit_code = DIVISION_ZERO;
                break;

            }
            _result = _num1 / _num2;
            break;
        case '%':
            if(_num2 == 0){
                _result = 0;
                _exit_code = DIVISION_ZERO;
                break;
            }
            _result = _num1 % _num2;
            break;
        default:
            _result = 0;
            _exit_code = OP_ERROR;
        }
    }

    std::string GetTask(){
        std::string r = std::to_string(_num1);
        r += _op;
        r += std::to_string(_num2);
        r += "=?";
        return r;
    }

    std::string GetResult()
    {
        std::string r = std::to_string(_num1);
        r += _op;
        r += std::to_string(_num2);
        r += "=";
        r += std::to_string(_result);
        r += "[code: ";
        r += std::to_string(_exit_code);
        r += "]";

        return r;
    }

    ~Task()
    {}

private:
    int _num1;
    int _num2;
    char _op;

    int _result;
    int _exit_code;
};