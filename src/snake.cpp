#include <snake.hpp>

Snake::Snake(int l_blockSize)
{
    m_size = l_blockSize;
    m_bodyRect.setSize(sf::Vector2f(m_size-1, m_size-1));
    Reset();
}

Snake::~Snake() = default;

void Snake::Reset()
{
    m_snakeBody.clear();

    m_snakeBody.push_back(SnakeSegment(5,7));
    m_snakeBody.push_back(SnakeSegment(5,6));
    m_snakeBody.push_back(SnakeSegment(5,5));

    SetDirection(Direction::None);
    m_speed = 15;
    m_lives = 3;
    m_score = 0;
    m_lost = false;
}

void Snake::SetDirection(Direction l_dir)
{
    m_dir = l_dir;
}

Direction Snake::GetDirection()
{
    return m_dir;
}

int Snake::GetSpeed()
{
    return m_speed;
}

sf::Vector2i Snake::GetPosition()
{
    return (!m_snakeBody.empty()) ? m_snakeBody.front().position : sf::Vector2i(1, 1);
}

int Snake::GetLives()
{
    return m_lives;
}

int Snake::GetScore()
{
    return m_score;
}

void Snake::IncreaseScore()
{
    m_score += 10;
}

bool Snake::HasLost()
{
    return m_lost;
}

void Snake::Lose()
{
    m_lost = true;
}

void Snake::ToggleLost()
{
    m_lost = !m_lost;
}

void Snake::Extend()
{
    if (m_snakeBody.empty()){ return; }

    SnakeSegment& tail_head =
    m_snakeBody[m_snakeBody.size() - 1];

    if(m_snakeBody.size() > 1)
    {
        SnakeSegment& tail_bone = m_snakeBody[m_snakeBody.size() - 2];
        if(tail_head.position.x == tail_bone.position.x)
        {
            if(tail_head.position.y > tail_bone.position.y)
            {
                m_snakeBody.push_back(SnakeSegment(
                tail_head.position.x, tail_head.position.y + 1));
            }
            else
            {
                m_snakeBody.push_back(SnakeSegment( tail_head.position.x, tail_head.position.y - 1));
            }
        } 
        else if(tail_head.position.y == tail_bone.position.y)
        {
            if(tail_head.position.x > tail_bone.position.x)
            {
                m_snakeBody.push_back(SnakeSegment(
                tail_head.position.x + 1, tail_head.position.y));
            }
            else 
            {
                m_snakeBody.push_back(SnakeSegment(
                tail_head.position.x - 1, tail_head.position.y));
            }
        }
    }
    else 
    {
        if(m_dir == Direction::Up)
        {
            m_snakeBody.push_back(SnakeSegment( tail_head.position.x, tail_head.position.y + 1));
        }
        else if (m_dir == Direction::Down)
        {
            m_snakeBody.push_back(SnakeSegment( tail_head.position.x, tail_head.position.y - 1));
        }
        else if (m_dir == Direction::Left)
        {
            m_snakeBody.push_back(SnakeSegment(tail_head.position.x + 1, tail_head.position.y));
        }
        else if (m_dir == Direction::Right)
        {
            m_snakeBody.push_back(SnakeSegment( tail_head.position.x - 1, tail_head.position.y));
        }
    }
}

void Snake::Tick()
{
    if(m_snakeBody.empty()) { return;}
    if(m_dir == Direction::None) { return; }
    Move();
    CheckCollision();
}

void Snake::Move()
{
    for(int i = m_snakeBody.size() - 1; i > 0; --i)
    {
        m_snakeBody[i].position = m_snakeBody[i-1].position;
    }

    switch(m_dir)
    {
        case Direction::Up:
            --m_snakeBody[0].position.y;
            break;
        case Direction::Down:
            ++m_snakeBody[0].position.y;
            break;
        case Direction::Left:
            --m_snakeBody[0].position.x;
            break;
        case Direction::Right:
            ++m_snakeBody[0].position.x;
            break;
        default:
            break;
    }
}

void Snake::CheckCollision()
{
    if(m_snakeBody.size() < 5) { return;}

    SnakeSegment& head = m_snakeBody.front();
    for(auto itr = m_snakeBody.begin() + 1; itr != m_snakeBody.end(); ++itr)
    {
        if(itr->position == head.position)
        {
            int segments = std::distance(itr, m_snakeBody.end());
            Cut(segments);
        }
    }
}

void Snake::Cut(int l_segments)
{
    for(int i = 0; i < l_segments; ++i)
    {
        m_snakeBody.pop_back();
    }

    --m_lives;
    if(!m_lives)
    {
        Lose();
        return;
    }
}

void Snake::Render(sf::RenderWindow& l_window)
{
    if(m_snakeBody.empty()) { return; }

    auto head = m_snakeBody.begin();
    m_bodyRect.setFillColor(sf::Color::Yellow);
    m_bodyRect.setPosition(head->position.x * m_size, head->position.y * m_size);
    l_window.draw(m_bodyRect);

    m_bodyRect.setFillColor(sf::Color::Green);
    for(auto itr = m_snakeBody.begin() + 1; itr != m_snakeBody.end(); ++itr)
    {
        m_bodyRect.setPosition(itr->position.x * m_size, itr->position.y * m_size);
        l_window.draw(m_bodyRect);
    }
}