#include <DgEngine/Inc/DgEngine.h>

using namespace DgEngine;
using namespace DgEngine::Core;

class Bullet
{
public:
    Bullet()
        : damage(10), speed(20.0f)
    {
        LOG("Bullet default ctor | damage=%d speed=%.2f", damage, speed);
    }

    ~Bullet()
    {
        LOG("Bullet dtor | damage=%d speed=%.2f", damage, speed);
    }

    int damage = 0;
    float speed = 0.0f;
};

class Enemy
{
public:
    Enemy()
        : health(100), level(1)
    {
        LOG("Enemy default ctor | health=%d level=%d", health, level);
    }

    Enemy(int h, int l)
        : health(h), level(l)
    {
        LOG("Enemy overloaded ctor | health=%d level=%d", health, level);
    }

    ~Enemy()
    {
        LOG("Enemy dtor | health=%d level=%d", health, level);
    }

    int health = 0;
    int level = 0;
};

class Particle
{
public:
    Particle()
        : life(1.0f), x(0.0f), y(0.0f)
    {
        LOG("Particle default ctor | life=%.2f pos=(%.2f, %.2f)", life, x, y);
    }

    ~Particle()
    {
        LOG("Particle dtor | life=%.2f pos=(%.2f, %.2f)", life, x, y);
    }

    float life = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
};

int WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    LOG("===== OBJECT POOL TEST START =====");

    constexpr size_t kCapacity = 100;

    TypedAllocator<Bullet> bulletPool("BulletPool", kCapacity);
    TypedAllocator<Enemy> enemyPool("EnemyPool", kCapacity);
    TypedAllocator<Particle> particlePool("ParticlePool", kCapacity);

    std::vector<Bullet*> bullets;
    std::vector<Enemy*> enemies;
    std::vector<Particle*> particles;

    bullets.reserve(kCapacity);
    enemies.reserve(kCapacity);
    particles.reserve(kCapacity);

    LOG("===== ADD 70 TO EACH POOL =====");

    for (int i = 0; i < 70; ++i)
    {
        bullets.push_back(bulletPool.New());

        enemies.push_back(enemyPool.New(100 + i, (i % 5) + 1));

        particles.push_back(particlePool.New());
    }

    LOG("===== DELETE 40 FROM EACH POOL =====");

    for (int i = 0; i < 40; ++i)
    {
        bulletPool.Delete(bullets.back());
        bullets.pop_back();

        enemyPool.Delete(enemies.back());
        enemies.pop_back();

        particlePool.Delete(particles.back());
        particles.pop_back();
    }

    LOG("===== FILL EACH POOL TO CAPACITY =====");

    while (bullets.size() < kCapacity)
    {
        bullets.push_back(bulletPool.New());
    }

    while (enemies.size() < kCapacity)
    {
        enemies.push_back(enemyPool.New(250, 10));
    }

    while (particles.size() < kCapacity)
    {
        particles.push_back(particlePool.New());
    }

    LOG("===== CLEAN UP ALL OBJECTS =====");

    for (Bullet* ptr : bullets)
    {
        bulletPool.Delete(ptr);
    }
    bullets.clear();

    for (Enemy* ptr : enemies)
    {
        enemyPool.Delete(ptr);
    }
    enemies.clear();

    for (Particle* ptr : particles)
    {
        particlePool.Delete(ptr);
    }
    particles.clear();

    LOG("===== OBJECT POOL TEST END =====");
    return 0;
}

//static int sUniqueId = 0;
//class Student
//{
//public:
//	Student()
//	{
//		mId = ++sUniqueId;
//		mMark = 60 + (rand() % 41);
//		mName = "noName";
//	}
//	void Log()
//	{
//		LOG("Name: %s - ID: %d - Mark: %d", mName.c_str(), mId, mMark);
//	}
//private:
//	int mId = 0;
//	int mMark = 0;
//	std::string mName;
//};
//
//int WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
//{
//	TypedAllocator studentPool = TypedAllocator<Student>("StudentPool", 100);
//
//	std::vector<Student>
//	return 0; 
//}