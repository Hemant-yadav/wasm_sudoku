#include <stdbool.h>
#include <string.h>

#define CELL_LENGTH 3
#define LENGTH (CELL_LENGTH * CELL_LENGTH)
#define TOTAL (LENGTH * LENGTH)
#define MASK ((2 << LENGTH) - 1)

void filter_cell(char *sudoku, short *map, char i, char j)
{
  char e = sudoku[i * LENGTH + j];
  // Filter row and column
  for (char k = 0; k < LENGTH; k++)
  {
    map[k * LENGTH + j] &= ~(1 << e);
    map[i * LENGTH + k] &= ~(1 << e);
  }
  // Filter cell grid
  char lm = i / CELL_LENGTH * CELL_LENGTH;
  char ln = j / CELL_LENGTH * CELL_LENGTH;
  for (char m = 0; m < CELL_LENGTH; m++)
    for (char n = 0; n < CELL_LENGTH; n++)
      map[(lm + m) * LENGTH + ln + n] &= ~(1 << e);

  // Mark as filtered
  map[i * LENGTH + j] = 1;
}
bool filter(char *sudoku, short *map)
{
  bool updated = false;
  for (char i = 0; i < LENGTH; i++)
  {
    for (char j = 0; j < LENGTH; j++)
    {
      if ((map[i * LENGTH + j] & 1) && (map[i * LENGTH + j] != 1))
      {
        char e = sudoku[i * LENGTH + j];
        updated = true;
        filter_cell(sudoku, map, i, j);
      }
    }
  }

  return updated;
}

short fill(char *sudoku, short *map, short *meta)
{
  short updated = 0;
  char count = 0;
  char v;

  for (char k = 1; k <= LENGTH; k++)
  {
    for (char d = 0; d < LENGTH; d++)
    {
      // If only posible value on that row
      if (!(meta[d] & (1 << k)))
      {
        count = 0;
        for (char c = 0; c < LENGTH; c++)
        {
          if (map[d * LENGTH + c] & (1 << k))
          {
            v = d * LENGTH + c;
            count++;
          }
        }
        if (count == 1)
        {
          updated++;
          sudoku[v] = k;

          map[v] |= 1;
          meta[d] |= (1 << k);
          filter_cell(sudoku, map, d, v % LENGTH);
        }
      }
      // If only posible value on that column
      if (!(meta[LENGTH + d] & (1 << k)))
      {
        count = 0;
        for (char c = 0; c < LENGTH; c++)
        {
          if (map[c * LENGTH + d] & (1 << k))
          {
            v = c * LENGTH + d;
            count++;
          }
        }
        if (count == 1)
        {
          updated++;
          sudoku[v] = k;

          map[v] |= 1;
          meta[LENGTH + d] |= (1 << k);
          filter_cell(sudoku, map, v / LENGTH, d);
        }
      }
      // If only posible value on cell grid
      if (!(meta[LENGTH + LENGTH + d] & (1 << k)))
      {
        count = 0;
        for (char c = 0; c < LENGTH; c++)
        {
          if (map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                  d % CELL_LENGTH * CELL_LENGTH +
                  c / CELL_LENGTH * LENGTH +
                  c % CELL_LENGTH] &
              (1 << k))
          {
            v = d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                d % CELL_LENGTH * CELL_LENGTH +
                c / CELL_LENGTH * LENGTH +
                c % CELL_LENGTH;
            count++;
          }
        }
        if (count == 1)
        {
          updated++;
          sudoku[v] = k;

          map[v] |= 1;
          meta[LENGTH + LENGTH + d] |= (1 << k);
          filter_cell(sudoku, map, v / LENGTH, v % LENGTH);
        }
      }
    }
  }

  for (char i = 0; i < LENGTH; i++)
  {
    for (char j = 0; j < LENGTH; j++)
    {
      if (map[i * LENGTH + j] != 1)
      {
        count = 0;
        // If only posible value on that cell
        for (char k = 1; k <= LENGTH; k++)
        {
          if (map[i * LENGTH + j] & (1 << k))
          {
            v = k;
            count++;
          }
        }
        if (count == 1)
        {
          updated++;
          sudoku[i * LENGTH + j] = v;
          map[i * LENGTH + j] |= 1;
          filter_cell(sudoku, map, i, j);
        }
      }
    }
  }
  return updated;
}
short countBits(short n)
{
  short count = 0;
  while (n)
  {
    count++;
    n >>= 1;
  }
  return count;
}

bool multiline(char *sudoku, short *map, short *meta)
{
  bool updated = false;
  for (char d = 0, group_depth = LENGTH - 2; d < LENGTH; d++)
  {
    // Checking all rows
    if ((meta[d] & (MASK | 1)) != MASK)
    {
      short histo[LENGTH] = {0};
      for (char c = 0; c < LENGTH; c++)
      {
        if (map[d * LENGTH + c] & 1)
          continue;
        short number = map[d * LENGTH + c] >> 1;
        for (short i = 0; number > 0; i++)
        {
          if (number & 1)
            histo[i]++;
          number >>= 1;
        }
      }
      // All groups from 2 to LENGTH - 1
      for (char c = 2; c < group_depth; c++)
      {
        // All values from 0 (1 << 0 => 1) to LENGTH
        for (char i = 0; i < LENGTH; i++)
        {
          if (histo[i] != c)
            continue;
          short mux = MASK;
          for (char j = 0; j < LENGTH; j++)
          {
            if (map[d * LENGTH + j] & (2 << i))
            {
              mux &= map[d * LENGTH + j];
            }
          }
          if (countBits(mux >> 1) > c)
          {
            char member_count = 1;
            short fix = 2 << i;
            for (char j = i + 1; member_count < c && j < LENGTH; j++)
            {
              if (histo[j] != c)
                continue;
              if (mux & (2 << j))
              {
                member_count++;
                fix |= 2 << j;
              }
            }
            if (member_count == c)
            {
              updated = true;
              for (char j = 0; j < LENGTH; j++)
              {
                if (map[d * LENGTH + j] & (2 << i))
                {
                  map[d * LENGTH + j] &= fix;
                }
                else
                {
                  map[d * LENGTH + j] &= ~(fix);
                }
              }
            }
          }
        }
      }
    }
    // Checking all columns
    if ((meta[LENGTH + d] & (MASK | 1)) != MASK)
    {
      short histo[LENGTH] = {0};
      for (char c = 0; c < LENGTH; c++)
      {
        if (map[c * LENGTH + d] & 1)
          continue;
        short number = map[c * LENGTH + d] >> 1;
        for (short i = 0; number > 0; i++)
        {
          if (number & 1)
            histo[i]++;
          number >>= 1;
        }
      }
      // All groups from 2 to LENGTH - 1
      for (char c = 2; c < group_depth; c++)
      {
        // All values from 0 (1 << 0 => 1) to LENGTH
        for (char i = 0; i < LENGTH; i++)
        {
          if (histo[i] != c)
            continue;
          short mux = MASK;
          for (char j = 0; j < LENGTH; j++)
          {
            if (map[j * LENGTH + d] & (2 << i))
            {
              mux &= map[j * LENGTH + d];
            }
          }
          if (countBits(mux >> 1) > c)
          {
            char member_count = 1;
            short fix = 2 << i;
            for (char j = i + 1; member_count < c && j < LENGTH; j++)
            {
              if (histo[j] != c)
                continue;
              if (mux & (2 << j))
              {
                member_count++;
                fix |= 2 << j;
              }
            }
            if (member_count == c)
            {
              updated = true;
              for (char j = 0; j < LENGTH; j++)
              {
                if (map[j * LENGTH + d] & (2 << i))
                {
                  map[j * LENGTH + d] &= fix;
                }
                else
                {
                  map[j * LENGTH + d] &= ~(fix);
                }
              }
            }
          }
        }
      }
    }
    // Checking all cell grids
    if ((meta[LENGTH + LENGTH + d] & (MASK | 1)) != MASK)
    {
      short histo[LENGTH] = {0};
      for (char c = 0; c < LENGTH; c++)
      {
        if (map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                d % CELL_LENGTH * CELL_LENGTH +
                c / CELL_LENGTH * LENGTH +
                c % CELL_LENGTH] &
            1)
          continue;
        short number = map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                           d % CELL_LENGTH * CELL_LENGTH +
                           c / CELL_LENGTH * LENGTH +
                           c % CELL_LENGTH] >>
                       1;
        for (short i = 0; number > 0; i++)
        {
          if (number & 1)
            histo[i]++;
          number >>= 1;
        }
      }
      // All groups from 2 to LENGTH - 1
      for (char c = 2; c < group_depth; c++)
      {
        // All values from 0 (1 << 0 => 1) to LENGTH
        for (char i = 0; i < LENGTH; i++)
        {
          if (histo[i] != c)
            continue;
          short mux = MASK;
          for (char j = 0; j < LENGTH; j++)
          {
            if (map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                    d % CELL_LENGTH * CELL_LENGTH +
                    j / CELL_LENGTH * LENGTH +
                    j % CELL_LENGTH] &
                (2 << i))
            {
              mux &= map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                         d % CELL_LENGTH * CELL_LENGTH +
                         j / CELL_LENGTH * LENGTH +
                         j % CELL_LENGTH];
            }
          }
          if (countBits(mux >> 1) > c)
          {
            char member_count = 1;
            short fix = 2 << i;
            for (char j = i + 1; member_count < c && j < LENGTH; j++)
            {
              if (histo[j] != c)
                continue;
              if (mux & (2 << j))
              {
                member_count++;
                fix |= 2 << j;
              }
            }
            if (member_count == c)
            {
              updated = true;
              for (char j = 0; j < LENGTH; j++)
              {
                if (map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                        d % CELL_LENGTH * CELL_LENGTH +
                        j / CELL_LENGTH * LENGTH +
                        j % CELL_LENGTH] &
                    (2 << i))
                {
                  map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                      d % CELL_LENGTH * CELL_LENGTH +
                      j / CELL_LENGTH * LENGTH +
                      j % CELL_LENGTH] &= fix;
                }
                else
                {
                  map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                      d % CELL_LENGTH * CELL_LENGTH +
                      j / CELL_LENGTH * LENGTH +
                      j % CELL_LENGTH] &= ~(fix);
                }
              }
            }
          }
        }
      }
    }
  }
  return updated;
}
void *solver(char *sudoku, short *map, short *meta)
{
  memset(meta, 0, 3 * LENGTH * sizeof(*meta));
  short remaining = LENGTH * LENGTH;
  for (char i = 0; i < TOTAL; i++)
    map[i] = (MASK ^ 1) | (sudoku[i] && remaining--);
  // filter(sudoku, map);
  short v;
  for (char loopO = 1; remaining > 0 && loopO > 0; loopO--)
  {
    for (char vd = 1; remaining > 0 && vd > 0; vd--)
    {
      if (filter(sudoku, map))
      {
        vd = 2;
      }
      v = fill(sudoku, map, meta);
      if (v)
      {
        remaining -= v;
        vd = 2;
      }
    }
    if (remaining)
    {
      multiline(sudoku, map, meta);
      v = fill(sudoku, map, meta);
      if (v)
      {
        remaining -= v;
        loopO = 2;
      }
    }
  }
}