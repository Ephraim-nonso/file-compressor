FROM node:22-alpine AS build

WORKDIR /app

COPY package.json package-lock.json* ./
RUN npm ci

COPY . .
RUN npm run build

# Runtime image
FROM node:22-alpine AS runner

WORKDIR /app

ENV NODE_ENV=production
ENV PORT=3000

# Waku docs: for Docker/standalone, dist is the build output and can be served with node dist/serve-node.js
COPY --from=build /app/dist ./dist

EXPOSE 3000

CMD ["node", "dist/serve-node.js"]




